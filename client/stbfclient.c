/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.csource.org/ for more detail.
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "fdfs_client.h"
#include "logger.h"
#include "fdfs_global.h"

int stbf_test() { return 0; }

static char g_conf_filename[PATH_MAX] = {"/etc/fdfs/client.conf"};

int stbf_setconf(const char* conf_filename) {
    if (strlen(conf_filename) != 0) {
        strcpy(g_conf_filename, conf_filename);
	
	int result = 0;
    	if ((result = fdfs_client_init(g_conf_filename)) != 0) {
        	return 1;
    	}
        return 0;
    } else {
        return 1;
    }
}



int stbf_upload(const char* local_filename, char* ret_file_id) {
    char group_name[FDFS_GROUP_NAME_MAX_LEN + 1] = {0};
    ConnectionInfo* pTrackerServer;
    int result = 0;
    int store_path_index;
    ConnectionInfo trackerserver,storageServer;
    char file_id[128] = {0};

    log_init();
    g_log_context.log_level = LOG_ERR;
    ignore_signal_pipe();

    pTrackerServer = &trackerserver; 
    tracker_get_connection_r(&trackerserver, &result);
    
    if(result != 0) {
        //fdfs_client_destroy();
        return errno != 0 ? errno : ECONNREFUSED;
    }


    *group_name = '\0';
    if ((result = tracker_query_storage_store(pTrackerServer, &storageServer,
                                              group_name, &store_path_index)) !=
        0) {
        //fdfs_client_destroy();
        fprintf(stderr,
                "tracker_query_storage fail, "
                "error no: %d, error info: %s\n",
                result, STRERROR(result));
        return result;
    }

    result = storage_upload_by_filename1(pTrackerServer, &storageServer,
                                         store_path_index, local_filename, NULL,
                                         NULL, 0, group_name, file_id);
    if (result == 0) {
        strcpy(ret_file_id, file_id);
    } else {
        fprintf(stderr,
                "upload file fail, "
                "error no: %d, error info: %s\n",
                result, STRERROR(result));
    }

    tracker_disconnect_server(&storageServer);
    tracker_disconnect_server(&trackerserver);
    //tracker_disconnect_server_ex(pTrackerServer, true);
    return result;
}

int stbf_download(const char* fdfs_id, char* local_path) {
    //char group_name[FDFS_GROUP_NAME_MAX_LEN + 1] = {0};
    char* path_name = NULL;
    char* local_filename = NULL;
    ConnectionInfo* pTrackerServer;
    //ConnectionInfo *pStorageServer;
    ConnectionInfo trackerserver;
    //ConnectionInfo storageServer;
    //int store_path_index = 0;
    int result = 0;
    int64_t file_size;
    int64_t file_offset;
    int64_t download_bytes;

    log_init();
    g_log_context.log_level = LOG_ERR;
    ignore_signal_pipe();

    pTrackerServer = &trackerserver;
    tracker_get_connection_r(&trackerserver, &result);
    if(result != 0){    
        //fdfs_client_destroy();
        return errno != 0 ? errno : ECONNREFUSED;
    }

    /**group_name = '\0';
    if ((result = tracker_query_storage_store(pTrackerServer, &storageServer,
                                              group_name, &store_path_index)) !=
        0) {
        //fdfs_client_destroy();
        fprintf(stderr,
                "tracker_query_storage fail, "
                "error no: %d, error info: %s\n",
                result, STRERROR(result));
        return result;
    }

    tracker_connect_server(&storageServer, &result);
    if(result != 0)
    {
        fprintf(stderr,
                "tracker_connect_server to storage server fail, "
                "error no: %d, error info: %s\n",
                result, STRERROR(result));
        return result;
    }*/
	

    file_offset = 0;
    download_bytes = 0;
    local_filename = strrchr(fdfs_id, '/');
    if (local_filename != NULL) {
        local_filename++;
    } else {
        local_filename = (char*)(fdfs_id);
    }
	
    path_name = malloc(PATH_MAX);
    sprintf(path_name, "%s/%s", local_path, local_filename);
    // SYZ, 20170704, local_filename -> path_name
    result = storage_do_download_file1_ex(
        pTrackerServer, NULL, FDFS_DOWNLOAD_TO_FILE, fdfs_id, file_offset,
        download_bytes, &path_name, NULL, &file_size);
    

    //result = storage_download_file_to_file1(pTrackerServer, pStorageServer, fdfs_id, &path_name, &file_size);
    if (result != 0) {
        printf(
            "download file '%s' fail, "
            "error no: %d, error info: %s\n",
            local_filename, result, STRERROR(result));
    }
    //printf("%s\n", path_name);

    //tracker_disconnect_server(pStorageServer);
    tracker_disconnect_server(&trackerserver);

    free(path_name);

    return result;
}

int stbf_delete(const char* fdfs_id) {
    ConnectionInfo* pTrackerServer;
    ConnectionInfo trackerserver;
    int result = 0;

    log_init();
    g_log_context.log_level = LOG_ERR;
    ignore_signal_pipe();

    pTrackerServer = &trackerserver;
    tracker_get_connection_r(&trackerserver, &result);
    if(result != 0){    
        //fdfs_client_destroy();
        return errno != 0 ? errno : ECONNREFUSED;
    }


    if ((result = storage_delete_file1(pTrackerServer, NULL, fdfs_id)) != 0) {
        printf(
            "delete file fail, "
            "error no: %d, error info: %s\n",
            result, STRERROR(result));
    }

    tracker_disconnect_server(&trackerserver);

    return result;
}

void stbf_release() {
    fdfs_client_destroy();
}
