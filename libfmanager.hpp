#ifndef FMANAGER_H
#define FMANAGER_H

/******************************************************
 * copyright 2009, 2010, 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
 * file managment class
******************************************************/

// accoarding to the google coding style , iam on the wrong side of the world
// but hey , i don't care

//		BSD License
//      Redistribution and use in source and binary forms, with or without
//      modification, are permitted provided that the following conditions are
//      met:
//      
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above
//        copyright notice, this list of conditions and the following disclaimer
//        in the documentation and/or other materials provided with the
//        distribution.
//      * Neither the name of the  nor the names of its
//        contributors may be used to endorse or promote products derived from
//        this software without specific prior written permission.
//      
//      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//      "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//      LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//      A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//      OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//      SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//      LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//      DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//      THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//      (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//      OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


/******************************************************
 * man , i need to fix up the coding style here :/
 * not multithreaded safe due to the shared stat and changeDirectory
 * add exceptions ? maybe streams too :D
 * setErrors before and return 1
 * add RE to searchDirectory ? :D
 * all errors are from posix man pages
 * see LFS "large file support" for larger than 2gb
******************************************************/

#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <limits.h>
#include <errno.h>
#include <dirent.h>
#include <map>
#include <sstream>
#include <unistd.h>

#ifdef linux
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#endif


/*
#ifdef windows
#include laters
#endif
*/

using namespace std;


struct TIME
{
	int sec;
	int min;
	int hour;
	int mday;
	int mon;
	int year;
	int wday;
	int yday;
	int isdst;
	string sDate;
	TIME() : sec(0), min(0), hour(0), mday(0), mon(0), year(0), wday(0), yday(0), isdst(0){};
};


struct FINFO
{
	string	Name;	// name
	int		Type;	// Dir or File
	int		mType;	// type , regular , link ,block ....
	int		Serial; // i-node number (serial number)
	int		Device;	
	
	int					GID; 	// group ID of owner
	int					UID;	// user ID of owner
	unsigned long		Size;	// size in bytes
	
	int		uPermissions; // Read, write, execute/search for user
	int		gPermissions; // Read, write, execute/search for group
	int		oPermissions; // Read, write, execute/search for others
	
	/*int		muPermissions; // Delete, change permission, and take ownership for owner
	int		mgPermissions; // Delete, change permission, and take ownership for groups
	int		moPermissions; // Delete, change permission, and take ownership for others*/
	
	
	// permissions Read, write, execute/search
	// 1 : read access
	// 2 : write access
	// 3 : read and write access
	// 4 : execute access
	// 5 : read and execute access
	// 6 : write and execute access
	// 7 : read,write and execute access
	// permissions for delete, change permissions and take ownership
	// 1 : Delete permission
	// 2 : Change permission permission
	// 3 : Take ownership permission
	
	int		nLinks;		// link counts to the file
	int		rDev;		//device number for special files
	TIME	aTime;		// time of last access
	TIME	mTime;		// time of last modifications
	TIME	cTime;		// time of last file status change
	int		blockSize;	// best I/O block size
	int		Blocks;		// number of disk blocks allocated
};

class fmanager
{
public:
		typedef enum {FILE = 3,DIRECTORY}TYPE;
		typedef enum {REG = 2, CHR, BLK, FIFO, LNK, SOCK}FILETYPE;
		
		// file functions  add functions to split and merge files
			///file definition
		int createFile(const string&, const bool&); // create empt file and if it exists overwrite it ( if bool is true ).
		int removeFile(const string&);
		int checkFile(const string&, const bool&); // check file existense
		int fileAttr(const string&); // return file premisisons and attributes
		FINFO* getFileInfo(const string&, const string&, const bool&, const bool&);
		FILETYPE fileType(const string&); // return file premisisons and attributes
		int checkAvailable(const string&); // check if a file or dir exists
			///file manipulation
		int moveFile(const string&, const string&); // also works as rename
		int copyFile(const string&, const string&, const bool&); // copy files
		string getFileData2(const string&, const unsigned long& = 0, const unsigned long& = 0); // return data as string from a file
		string getFileData(const string&); // return data as string from a file
			///Split and Merge
		int splitFile(const string&, const string&, const unsigned int&, const unsigned int&);
		int splitFilePart(const string&, const string&, const int&);
		int splitFileSize(const string&, const string&, const int&);
		int mergeFiles(const string&, const string&, const string&, const bool&);
		
		// directory functions
			///drirectory definition
		int createDirectory(const string&); // recrusive : can create parents too DIR1/DIR2
		int makeDirectory(const string&); // not-recursive : can only create child dir
			///directory manipulation
		int removeDirectory(const string&); // remove directory , takes arguments like ~/... or /a/b/c will remove c , or c:\\a\b\c or ./a or just a ( if the dir is in the curr working directory
		int checkDirectory(const string&, const bool&); // check directory existense
		int moveDirectory(const string&, const string&, const bool&); // also works as rename
		int copyDirectory(const string&, const string&, const bool&); // copy directories
		vector<string>* searchDirectory(const string&, const string&, const bool&, const bool&, const bool&); // search directory for dir or file , bool for recursive, bool for exact name or whatever contains it
		vector<FINFO*>* listDirectory(const string&, const bool&, const bool&, const string&); // dir name , recursive		
//		int walk_directory(string,void (*callback_func(void *))); // use it to walk and copy or delete dirs ?
		int changeDirectory(const string&); // change directory
		char* getCurrDir(); // get current directory
		
		// misc functions
		string getLDir(); // get launched from directory
		TYPE checkType(const string&);
		
		// helper functions
		bool isFullPath(const string&); // given path is full
		string getFullPath(const string&, const bool&); // get full path of file ( if bool false) or dir
		string getTopPath(const string&);
		string getChildPath(const string&);
		string cleanPath(const string); // remove pathSeperator from the end of a string
				
		// error handeling
		string getError(); // get last error
		int clearError(); // set last error

		//class initiliaze and uninitilaize functions
		fmanager(const string&); // initialize an instance of fmanager with a name
		~fmanager();


		#ifdef linux
		enum mountOptions
		{
			NO_FLAGS_AND_DATA = MS_MGC_VAL,	// This multibit field contains a magic number. If it does not have the value MS_MGC_VAL, mount assumes all the following bits are zero and the data argument is a null string, regardless of their actual values. 
			READ_ONLY = MS_RDONLY,			// no writing to the filesystem shall be allowed while it is mounted. This cannot be overridden by ioctl. This option is available on nearly all filesystems. 
			REMOUNT = MS_REMOUNT,			// remount the filesystem. Off means to mount it. 
			NO_DEV = MS_NODEV,				// no device special files in the filesystem shall be accessible while the filesystem is mounted. 
			NO_EXEC = MS_NOEXEC,			// no files in the filesystem shall be executed while the filesystem is mounted. 
			NO_SUID = MS_NOSUID,			// Setuid and Setgid permissions on files in the filesystem shall be ignored while it is mounted. 
			NO_ATIME,						// access times of files shall not be updated when the files are accessed while the filesystem is mounted. 
			NO_DIRATIME						// access times of directories shall not be updated when the directories are accessed while the filesystem in mounted. 
			//FS_IMMUTABLE = S_IMMUTABLE,	// no writing to the files in the filesystem shall be allowed while it is mounted. This can be overridden for a particular file access by a properly privileged call to ioctl. This option is a relatively new invention and is not available on many filesystems. 
			//FS_ONLY_APPED = S_APPEND		// the only file writing that shall be allowed while the filesystem is mounted is appending. Some filesystems allow this to be overridden for a particular process by a properly privileged call to ioctl. This is a relatively new invention and is not available on many filesystems. 
		};
		
		int Mount(const string&, const string&, const string&, int);
		int unMount(const string&, bool);
		#endif //linux

private:
		int initSymbols();
		string fmname;
		string ldir; // launched from directory
		char buff[PATH_MAX];
		
		struct stat* Stat;
		
		string error;
		string osv; // Operating System Versoin
		
		int setError(const string&); // set last error
		int setError(); // set last error
		
		int doStat(const string&); // call stat
		int resetStat(); // delete Stat
		
		string pathSeperator;
};
#endif //FMANAGER_H
