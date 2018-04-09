#ifndef SEETABRAIN_UPLOAD_H_
#define SEETABRAIN_UPLOAD_H_

#ifdef __cplusplus
extern "C" {
#endif

extern int stbf_test();
extern int stbf_setconf(const char* conf_pathname);
extern int stbf_upload(const char* local_filename, char* ret_file_id);
extern int stbf_download(const char* fdfs_id, const char* local_path);
extern int stbf_delete(const char* fdfs_id);
extern int stbf_release();

#ifdef __cplusplus
}
#endif

#endif
