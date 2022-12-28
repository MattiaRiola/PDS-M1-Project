# PDS-M1-Project
PDS Project malnati (m1) Remote backup

## Description

in this repository there are:
- tcp
  - In this folder there is the server where the beckup is stored it must be started before the client, all the files of the client will be stored inside this folder
- testclient
  - In this folder there is the client where the files stored in the clientdirectory will be analyzed and sent to the server side. (a file is considered changed if its md5 changes)
