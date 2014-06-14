else if (strcmp(command, "UPDATE") == 0) {
		char *returnvalue = malloc(sizeof(char) * MAX_FILE_LENGTH);
		LOG_TRACE(LOG_INFORMATIONAL, "Will no try to UPDATE the file \"%i\"", recMessage[2]);
		int retcode;

		/* check if filename was given */
		if (recMessage[2] == NULL) {
			LOG_TRACE(LOG_INFORMATIONAL, "Can not update a file without a name");
			returnvalue = "Can not update a file without a name";
			send(clntSocket, returnvalue, strlen(returnvalue), 0);
		}

		/* check if new content was given*/
		else if (recMessage[3] == NULL) {
			LOG_TRACE(LOG_INFORMATIONAL, "Can not update a file without any content");
			returnvalue = "Can not update a file without any content";
			send(clntSocket, returnvalue, strlen(returnvalue), 0);
		}

		else {

			retcode = checkifexists(shm_ctr, recMessage[2]);

			/* if file does not exist, send message to Client */
			if (!retcode) {
				sendtoClient = getSingleString("File with the name \"%s\" does not exist!\n", recMessage[2]);
				send(clntSocket, sendtoClient, strlen(sendtoClient), 0);
			}

			/* if file exist, delete it and create it new */
			else {
				retcode = deleteFile(shm_ctr, recMessage[2]);
				/* if deleting was successful, call runClientCommand and */
				if (retcode) {

					/* combine now the free blocks */
					retcode = combine(shm_ctr);
					/* repeat until there is no more deviding option */
					while (retcode == TRUE) {
						retcode = combine(shm_ctr);
					}

					/* create the new file with the content */
					char *tmpcontent = (char *) malloc(MAX_FILE_LENGTH);
					tmpcontent = getFileContent(recMessage);
					char *filecontent = strdup(tmpcontent);
					free(tmpcontent);

					char *filename = strdup(recMessage[2]);
					LOG_TRACE(LOG_INFORMATIONAL, "Filesize = %i \t Content = %s", (int) strlen(filecontent), filecontent);

					char *returnvalue = malloc(sizeof(char) * 256);
					returnvalue = createNewFile(shm_ctr, filename, filecontent, strlen(filecontent));
					if (returnvalue > 0) {
						returnvalue = getSingleString("File \"%s\" was successfully updated", filename);
						LOG_TRACE(LOG_INFORMATIONAL, "Sending message to Client: %s", returnvalue);
						send(clntSocket, returnvalue, strlen(returnvalue), 0);
					}
					free(returnvalue);

				}

				/*if deleting was not successful */
				else {
					LOG_TRACE(LOG_INFORMATIONAL, "Updating file \"%s\" was not successful", recMessage[2]);
					sendtoClient = getSingleString("Updating file \"%s\" was not successful", recMessage[2]);
					send(clntSocket, sendtoClient, strlen(sendtoClient), 0);
				}

			}

		}
	}
