#include <stdio.h>

#include <nfsc/libnfs.h>
#include <nfsc/libnfs-raw-mount.h>

#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


// globals, only one file at a time
struct nfs_context *nfs = NULL;
struct nfs_url     *url = NULL;
struct nfsfh       *fh  = NULL;
struct nfs_stat_64  st;
static uint64_t     off = 0;


int process_server(const char *server)
{
	printf("mount_getexports(%s):\n", server);
	struct exportnode *exports = NULL;
	struct exportnode *export = NULL;

	exports = mount_getexports(server);
	if (!exports)
	{
		printf("Failed to get exports for server %s.\n", server);
		return -1;
	}
	for (export=exports; export; export = export->ex_next) {
		printf("nfs://%s%s\n", server, export->ex_dir);
	}
	mount_free_export_list(exports);
	return 0;
}

// read chunks of max BUFSIZE in bytes
int nfs_client_read_buffer(unsigned char *pData, size_t pBufferSize)
{
    if(off < st.nfs_size)
	{
	    int count = st.nfs_size - off;
        if(count > pBufferSize) count = pBufferSize;
		
        count = nfs_pread(nfs, fh, off, count, pData);

        if(count < 0)
        {
        	printf("Failed to read from file\n");
        	return (-1);
        }

        off += count;
        return (count);
	}

	return 0;
}

int nfs_list_server_exports(const char *srv)
{
	// optional, list server exports
	int ret = process_server(srv);
	printf("process_server(nfs://%s): %d\n", srv, ret);
	
	return ret;
}

// setup nfs context, mount export
int nfs_client_init(const char *srv_url)
{
	nfs = nfs_init_context();

	nfs_set_debug(nfs, 0); /*debug_level*/

	// parse url and mount export
	url = nfs_parse_url_dir(nfs, srv_url);
    int ret = nfs_mount(nfs, url->server, url->path);
    printf("nfs_mount return: %d\n", ret);

	return ret;
}

// we have opened just one file at once
size_t nfs_client_stat_size(void)
{
    if(nfs_fstat64(nfs, fh, &st))
    {
    	printf("Failed to stat file : %s\n", nfs_get_error(nfs));
    	return -1;
    }
	return st.nfs_size;
}

// just open, don't read
int nfs_client_open(const char *filename)
{
	int flags = 0;
	off = 0;
	flags |= O_RDONLY;
	if(nfs_open(nfs, filename, flags, &fh))
	{
		printf("Failed to open(%s)\n", nfs_get_error(nfs));
		return -1;
	}
	return 1;
}

void nfs_client_close(void)
{
	printf("nfs_client_close, %p, %p, %p\n", nfs, url, fh);
    if(nfs_close(nfs, fh))
	{
		printf("Failed to close(): %s\n", nfs_get_error(nfs)); 
		return;
	}
}

// cleanup, umount export, destroy nfs context
void nfs_client_end(void)
{
	printf("nfs_client_end, %p, %p, %p\n", nfs, url, fh);

	if(!nfs)
		return;

    int ret = nfs_umount(nfs);
	printf("nfs_umount return: %x\n", ret);

	nfs_destroy_url(url);
	nfs_destroy_context(nfs);

	return;
}

int main(void)
{
	int read;
	size_t fsize;
	uint8_t buffer[1025];

	// List the available exports on the NFS server
	nfs_list_server_exports("192.168.10.123");

	// Mount an exported folder
	nfs_client_init("nfs://192.168.10.123/Exports/Shared");
 
 	// Open the file `test.txt` on the NFS server
	nfs_client_open("test.txt");
	
	// Get the file size
	fsize = nfs_client_stat_size();

	printf("File Size: %ld bytes\n", fsize);
	printf("File Content:\n");

	// Read the file contents
	do {
		read = nfs_client_read_buffer(buffer, 1024);
		if (read > 0)
		{
			buffer[read] = 0;
			printf("%s", buffer);
		}
	}
	while (read > 0);

	// Close the file, and end the connection
	nfs_client_close();
	nfs_client_end();
}
