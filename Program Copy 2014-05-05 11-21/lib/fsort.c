/* (C) IT Sky Consulting GmbH 2014
 * http://www.it-sky-consulting.com/
 * Author: Karl Brodowsky
 * Date: 2014-02-27
 * License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
 */

/* implements <a href="https://de.wikipedia.org/wiki/Flashsort">Flashsort</a> */

/* enable qsort_r */
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include <stdlib.h>
#include <errno.h>

#include <fsort.h>
#include <hsort.h>
#include <isort.h>
#include <itskylib.h>
#include <sortimpl.h>

void fsort(void *base,
           size_t nmemb,
           size_t size,
           compare_fun2 compare,
           metric_fun1 metric) {
  fsort_r(base, nmemb, size, compare_extend, (void *) compare, metric_extend, (void *) metric);
}


void fsort_r(void *base,
             size_t nmemb,
             size_t size,
             compare_fun3 compare,
             void *argc,
             metric_fun2 metric,
             void *argm) {
  fsort_f(base, nmemb, size, 0.42, compare, argc, metric, argm);
}

size_t calculate_k(double factor, double value, size_t lsize) {
  /* think of rounding errors, so add a little bit to the double */
  double prod = factor * value + 1e-9;
  if (prod < 0) {
    prod = 0;
  }
  if (prod > lsize -1) {
    prod = lsize - 1;
  }
  size_t result = (size_t) prod;
  return result;
}

void fsort_f(void *base,
             size_t nmemb,
             size_t size,
             double factor,
             compare_fun3 compare,
             void *argc,
             metric_fun2 metric,
             void *argm) {
  if (nmemb <= 1) {
    /* nothing to sort */
    return;
  }

  /* preparation: form classes */
  size_t lsize = calculate_k(nmemb, factor, nmemb) + 1;
  if (lsize < 2) {
    lsize = 2;
  }
  size_t *l = (size_t *) malloc(lsize * sizeof(size_t));
  handle_ptr_error(l, "malloc for l", PROCESS_EXIT);
  for (size_t k = 0; k < lsize; k++) {
    l[k] = 0;
  }
  //size_t idx_min = 0;
  void *amin = POINTER(base, 0, size);
  size_t idx_max = 0;
  void *amax = POINTER(base, idx_max, size);
  for (size_t i = 0; i < nmemb; i++) {
    if (compare(POINTER(base, i, size), amin, argc) < 0) {
      // idx_min = i;
      amin = POINTER(base, i, size);
    }
    if (compare(POINTER(base, i, size), amax, argc) > 0) {
      idx_max = i;
      amax = POINTER(base, i, size);
    }
  }
  if (compare(amin, amax, argc) == 0) {
    /* min and max are the same --> already sorted */
    free(l);
    return;
  }
  double amin_metric = metric(amin, argm);
  double amax_metric = metric(amax, argm);
  double step = (lsize - 1)/(amax_metric - amin_metric);
  /* size_t k_min = calculate_k(step, 0, lsize); */
  /* size_t k_max = calculate_k(step, amax_metric - amin_metric, lsize); */

  /* count the elements in each of the lsize classes */
  for (size_t i = 0; i < nmemb; i++) {
    double ai_metric = metric(POINTER(base, i, size), argm);
    size_t k = calculate_k(step, ai_metric - amin_metric, lsize);
    l[k]++;
  }

  /* find the start positions for each of the classes */
  size_t *ll = (size_t *) malloc((lsize + 1) * sizeof(size_t));
  handle_ptr_error(ll, "malloc for ll", PROCESS_EXIT);
  ll[0] = 0;
  ll[1] = l[0];

  for (size_t k = 1; k < lsize; k++) {
    l[k] += l[k-1];
    ll[k+1] = l[k];
  }

  swap_elements(POINTER(base, 0, size), POINTER(base, idx_max, size), size);
  // printf("prepared\n");

  /* do the permutation */
  size_t nmove = 0;
  size_t j = 0;
  size_t k = lsize - 1;
  while (nmove < nmemb - 1) {
    while (j >= l[k]) {
      j++;
      k = calculate_k(step, metric(POINTER(base, j, size), argm) - amin_metric, lsize);
    }
    /* now: j < l[k] */
    void *flash_ptr = alloca(size);
    handle_ptr_error(flash_ptr, "alloca for flash_ptr", PROCESS_EXIT);
    /* flash_ptr takes element a[j] such that j > l[k] */
    memcpy(flash_ptr, POINTER(base, j, size), size);
    while (j != l[k]) {
      k = calculate_k(step, metric(flash_ptr, argm) - amin_metric, lsize);
      void *alkm_ptr = POINTER(base, l[k]-1, size);
      swap_elements(flash_ptr, alkm_ptr, size);
      l[k]--;
      nmove++;
    }
  }

  /* use qsort or hsort for each class */
  for (k = 0; k < lsize; k++) {
    size_t n = ll[k+1] - ll[k];
    if (n > 1 && ll[k+1] < ll[k] || n > nmemb) {
      char txt[4096];
      sprintf(txt, "wrong order: k=%ld lsize=%ld nmemb=%ld n=%ld ll[k]=%ld ll[k+1]=%ld\n", k, lsize, nmemb, n, ll[k], ll[k+1]);
      handle_error_myerrno(-1, EDOM, txt, PROCESS_EXIT);
    }
    if (n > 7) {
      void *basek = POINTER(base, ll[k], size);
      hsort_r(basek, n, size, compare, argc);
    } else  if (n > 1) {
      void *basek = POINTER(base, ll[k], size);
      isort_r(basek, n, size, compare, argc);
    }
  }
  free(l);
  l = NULL;
  free(ll);
  ll = NULL;
  return;
}
