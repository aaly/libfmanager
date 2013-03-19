/******************************************************
 * copyright 2009, 2010, 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
 * file managment class
******************************************************/

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

#include "libfmanager.hpp"

int fmanager::initSymbols()
{
#ifdef linux
	osv = "linux";
	pathSeperator = "/";
#endif	

#ifdef windows
	osv = "windows";
	pathSeperator = "\\";
#endif	
	
	return 0;
};

fmanager::fmanager(const string& name) : fmname(name),  ldir(getcwd(buff,PATH_MAX)), Stat(new struct stat), error("")
{	
	initSymbols();
};

fmanager::~fmanager()
{
	delete Stat;
};

int fmanager::checkAvailable(const string& name)
{
	if (access(name.c_str(), F_OK) != -1)
	{
		return 0;
	}
	else
	{
		setError();
		return 1;
	}
};

int fmanager::checkFile(const string& filename, const bool& reset = true)
{
	doStat(filename);

	if( ! S_ISREG(Stat->st_mode) | S_ISCHR(Stat->st_mode) | S_ISBLK(Stat->st_mode) | S_ISFIFO(Stat->st_mode) | S_ISLNK(Stat->st_mode) | S_ISSOCK(Stat->st_mode) )
	//if(S_ISREG(fstat.st_mode))
	{
		return 1;
	}
	
	if (reset)
	{
		resetStat();
	}
	
	return 0;
};


fmanager::TYPE fmanager::checkType(const string& name)
{	
	struct stat fstat;
	if (lstat(name.c_str(), &fstat) == -1)
	{
		if (stat(name.c_str(), &fstat) == -1)
		{
			return (TYPE)1;
		}
		//return (1);
	}
 
	if (S_ISDIR(fstat.st_mode))
	{
		return DIRECTORY;
	}
	
	if(S_ISREG(fstat.st_mode) | S_ISCHR(fstat.st_mode) | S_ISBLK(fstat.st_mode) | S_ISFIFO(fstat.st_mode) | S_ISLNK(fstat.st_mode) | S_ISSOCK(fstat.st_mode))
	//if(S_ISREG(fstat.st_mode))
	{
		return FILE;
	}
	else
	{
		return (TYPE)1;
	}
	
};


int fmanager::fileAttr(const string& filename)
{
	int mode=0;
	if (access(filename.c_str(), F_OK) != -1)
	{
		if (access(filename.c_str(), R_OK) != -1)
		{
			mode+=1;
		}
		else if (access(filename.c_str(), W_OK) != -1)
		{
			mode+=3;
		}
		else if (access(filename.c_str(), X_OK) != -1)
		{
			mode+=5;
		}
	}
	else
	{
		return 1;
	}
	
	if ( mode == 1 )
	{
		return mode+1;
	}
	else if ( mode >= 2 && mode <= 9 )
	{
		return mode;
	}
	else
	{
		return 1;
	}

	// 2 : read access , returns 2 and not one since 1 is reserved for error
	// 3 : write access
	// 4 : read and write access
	// 5 : execute access
	// 6 : read and execute access
	// 8 : write and execute access
	// 9 : read,write and execute access
	// 1 : ERROR
};

FINFO* fmanager::getFileInfo(const string& fname, const string& date, const bool& stat = true, const bool& resetstat = true)
{
	if (stat)
	{
		doStat(fname);
	}
	
	struct tm* aTime = new struct tm;
	struct tm* mTime = new struct tm;
	struct tm* cTime = new struct tm;
	
	FINFO* fent = new FINFO;
			
	fent->Name = fname;
	fent->Size = Stat->st_size;
	fent->Serial = Stat->st_ino;
			
	// get date
	if (date == "gmt")
	{
		aTime = gmtime(&(Stat->st_atime));
		mTime = gmtime(&(Stat->st_mtime));
		cTime = gmtime(&(Stat->st_ctime));
	}
	else if (date == "local")
	{
		aTime = localtime(&(Stat->st_atime));
		mTime = localtime(&(Stat->st_mtime));
		cTime = localtime(&(Stat->st_ctime));
	}
			
	// store broken down date values
	fent->aTime.sec = aTime->tm_sec;
	fent->aTime.min = aTime->tm_min;
	fent->aTime.hour = aTime->tm_hour;
	fent->aTime.mday = aTime->tm_mday;
	fent->aTime.mon = aTime->tm_mon;
	fent->aTime.year = aTime->tm_year;
	fent->aTime.wday = aTime->tm_wday;
	fent->aTime.yday = aTime->tm_yday;
	fent->aTime.isdst = aTime->tm_isdst;

	fent->mTime.sec = mTime->tm_sec;
	fent->mTime.min = mTime->tm_min;
	fent->mTime.hour = mTime->tm_hour;
	fent->mTime.mday = mTime->tm_mday;
	fent->mTime.mon = mTime->tm_mon;
	fent->mTime.year = mTime->tm_year;
	fent->mTime.wday = mTime->tm_wday;
	fent->mTime.yday = mTime->tm_yday;
	fent->mTime.isdst = mTime->tm_isdst;
			
	fent->cTime.sec = cTime->tm_sec;
	fent->cTime.min = cTime->tm_min;
	fent->cTime.hour = cTime->tm_hour;
	fent->cTime.mday = cTime->tm_mday;
	fent->cTime.mon = cTime->tm_mon;
	fent->cTime.year = cTime->tm_year;
	fent->cTime.wday = cTime->tm_wday;
	fent->cTime.yday = cTime->tm_yday;
	fent->cTime.isdst = cTime->tm_isdst;
		
	// get string date
	fent->aTime.sDate = asctime(aTime);
	fent->mTime.sDate = asctime(mTime);
	fent->cTime.sDate = asctime(cTime);
			
	if (osv == "linux")
	{
		fent->Device = (char)(Stat->st_dev );
	}
	else if (osv == "windows")
	{
		 fent->Device = (char)(Stat->st_dev + 'A');
	}	
			
	fent->GID = Stat->st_gid;
	fent->UID = Stat->st_uid;
	fent->Size = Stat->st_size;
	fent->rDev = Stat->st_rdev;
			
	// set user permissions
	if (Stat->st_mode & S_IRWXU)
	{
		fent->uPermissions = 7;
	}
	else
	{
		if (Stat->st_mode & S_IRUSR)
		{
			fent->uPermissions += 1;
		}
				
		if (Stat->st_mode & S_IWUSR)
		{
			fent->uPermissions += 2;
		}
				
		if (Stat->st_mode & S_IXUSR)
		{
			fent->uPermissions += 3;
		}
	}

			
	// set groups permissions
	if (Stat->st_mode & S_IRWXG)
	{
		fent->gPermissions = 7;
	}
	else
	{
		if (Stat->st_mode & S_IRGRP)
		{
			fent->gPermissions += 1;
		}
				
		if (Stat->st_mode & S_IWGRP)
		{
			fent->gPermissions += 2;
		}
				
		if (Stat->st_mode & S_IXGRP)
		{
			fent->gPermissions += 3;
		}
	}

			
	// set others permissions
	if (Stat->st_mode & S_IRWXO)
	{
		fent->oPermissions = 7;
	}
	else
	{
		if (Stat->st_mode & S_IROTH)
		{
			fent->oPermissions += 1;
		}
			
		if (Stat->st_mode & S_IWOTH)
		{
			fent->oPermissions += 2;
		}
			
		if (Stat->st_mode & S_IXOTH)
		{
			fent->oPermissions += 3;
		}
	}

	fent->nLinks = Stat->st_nlink;
	fent->blockSize = Stat->st_blksize;
	fent->Blocks = Stat->st_blocks;
			
	if ( S_ISDIR(Stat->st_mode) )
	{
		fent->Type = DIRECTORY;
	}
	else
	{
		fent->Type = FILE;	
	}
		
	// set mType
		
	if ( S_ISREG(Stat->st_mode) )
	{
		fent->mType = REG;
	}
	else if ( S_ISCHR(Stat->st_mode) )
	{
		fent->mType = CHR;
	}
	else if ( S_ISBLK(Stat->st_mode) )
	{
		fent->mType = BLK;
	}
	else if ( S_ISFIFO(Stat->st_mode) )
	{
		fent->mType = FIFO;
	}
	else if ( S_ISLNK(Stat->st_mode) )
	{
		fent->mType = LNK;
	}
	else if ( S_ISSOCK(Stat->st_mode) )
	{
		fent->mType = SOCK;
	}
		
	if (resetstat)
	{
		if (resetStat())
		{
			return NULL;
		}
	}
	
	return fent;
};

int fmanager::changeDirectory(const string& dirname)
{
	if (chdir(dirname.c_str()) != 0)
	{
		return 1;
	}
	
	return 0;
};

fmanager::FILETYPE fmanager::fileType(const string& filename)
{
	
	struct stat fstat;
	if (stat(filename.c_str(), &fstat) == -1)
	{
		if (lstat(filename.c_str(), &fstat) == -1)
		{
			return (FILETYPE)1;
		}
	}
 
	if (S_ISREG(fstat.st_mode))
	{
		return REG;
	}
	else if (S_ISCHR(fstat.st_mode))
	{
		return CHR;
	}
	else if (S_ISBLK(fstat.st_mode))
	{
		return BLK;
	}
	else if (S_ISFIFO(fstat.st_mode))
	{
		return FIFO;
	}
	else if (S_ISLNK(fstat.st_mode))
	{
		return LNK;
	}
	else if (S_ISSOCK(fstat.st_mode))
	{
		return SOCK;
	}
	else
	{
		return (FILETYPE)1;
	}
};

char* fmanager::getCurrDir()
{
	if (getcwd(buff,PATH_MAX) == buff)
	{
		return buff;
	}
	else
	{
		return NULL;
	}
};


string fmanager::getLDir()
{
	return ldir;
};


int fmanager::removeFile(const string& fname)
{
	 if(remove(fname.c_str()) != 0 )
	 {
		 return 0;
	 }
	 else
	 {
		 return 1;
	 }
};

int fmanager::moveFile(const string& ofname ,const string& nfname) // the new file name , if it have dirs it must exist ! or created first!
{
	if (rename(ofname.c_str(),nfname.c_str()) == 0 )
	{
		return 0;
	}

	return 1;
};


int fmanager::createFile(const string& fname, const bool& force)
{
	if (!force)
	{
		if (checkFile(fname, true) == 0)
		{
			return 1;
		}
	}
	else
	{
		ofstream ofile;
		ofile.open(fname.c_str(), fstream::out);
		if (ofile.is_open())
		{
			ofile.close();
		}
		else
		{
			return 1;
		}
	}
	return 0;
};


int fmanager::copyFile(const string& ifname, const string& ofname, const bool& force)
{
	//TODO: add the same file error :|
	ifstream ifile;
	ofstream ofile;
	
	ifile.open(ifname.c_str(), fstream::in|fstream::binary);
	if (!ifile.is_open())
	{
		//setError("Couldn't create file :" + (string&)ofname + ", Already exists");
		return 1;
	}
	
	if (force)
	{
		removeFile(ofname);
	}
	
	ofile.open(ofname.c_str(), fstream::app);
	if (!ofile.is_open())
	{
		return 1;
	}
	
	ofile << ifile.rdbuf();
	
	ifile.close();
	ofile.close();
	
	return 0;
	
};


int fmanager::mergeFiles(const string& ifname, const string& ifname2, const string& ofname, const bool& force)
{
	
	if (force)
	{
		removeFile(ofname);
	}
	
	if ( copyFile(ifname, ofname, force) || copyFile(ifname2, ofname, false) )
	{
		return 1;
	}
	
	return 0;
	
};

int fmanager::splitFile(const string& fname, const string& ofname, const unsigned int& size, const unsigned int& type)
{
	FINFO* finfo = getFileInfo(fname, "local", true, true); // aha .. true true ..
	
	if ( ( finfo->Size <= size && type == 0) || size == 0 )
	{
		return 1;
	}
	
	int num; // number of splitted files
	
	if ( type == 0 ) // calcualte
	{
		float tmp = (finfo->Size/size);
		num = tmp;
		//if ( num != (float)finfo->Size/size + .5 )
		if(tmp != num)
		{
			++num;
		}
	}
	else if ( type == 1 ) // determined already by count not size
	{
		num = size;
	}
	else
	{
		return 1; // unknown type
	}
	
	int offset_array[num];
	//int size_array[num];
	int i;
	
	unsigned long partSize;
	if (type == 0)
	{
			partSize = size;
	}
	else if (type == 1)
	{
			partSize = finfo->Size/size;
	}

	/*if(type==0)
	{
		size_array[num-1] = finfo->Size - ( ( num - 1 ) * size );
	}*/
	
	
	for ( i = 0; i < num; i++ )
	{
		if (type == 0)
		{
			offset_array[i] = size*i;
		}
		else if (type == 1)
		{
			offset_array[i] = partSize*i;
		}
				
		fstream fs;
		stringstream sfname;
		sfname << ofname;
		sfname << ".";
		sfname << i;
		fs.open((sfname.str()).c_str(), ios::out);
		
		if (!fs)
		{
			return 1;
		}
		
		//string data(getFileData2(fname.c_str(), size_array[i], offset_array[i]));
		// bad bad bad, shouldn't get it all into memory, use a buffer :/
		//FIX check getFileData2 for errors
		fs << getFileData2(fname.c_str(), partSize, offset_array[i]);
		fs.close();
	}
	
	
	
	return 0;
};

int fmanager::splitFileSize(const string& fname, const string& ofname, const int& size)
{
	return splitFile(fname, ofname, size, 0);
};

int fmanager::splitFilePart(const string& fname, const string& ofname, const int& parts)
{
	return splitFile(fname, ofname, parts, 1);
};

int fmanager::doStat(const string& name)
{
	//if (!Stat)
	//{
	resetStat();
	//}
	
	if (lstat(name.c_str(), Stat) == -1)
	{
		if (stat(name.c_str(), Stat) == -1)
		{
			setError();
			return 1;
		}
	}	

	return 0;
};

int fmanager::checkDirectory(const string& dirname, const bool& reset = true)
{
	//struct stat dirstat;
	
	doStat(dirname);
	
	int status(0);
	
	if(!S_ISDIR(Stat->st_mode))
	{
		status = 1;
	}
	
	if (reset)
	{
		resetStat();
	}
	
	return status;
};

/*
int fmanager::createDirectory(const string& dirname) //legacy, what was i thinking ?
{
	int i=0, k=0;
	string tmp_dirname; // get full path too ?
	string back = getCurrDir();
	if (dirname.at(0) == '/')
	{
		k=1;
		changeDirectory("/");
	}
	else if (dirname.at(2) == '\\')
	{
		k=3;
		changeDirectory(dirname.substr(0,2));
	}

	for (i=k; i<(int)dirname.length(); i++)
	{
		if ( ( dirname.at(i) == '/' ) || ( dirname.at(i) == '\\' ) || ( i == ((int)dirname.length() -1) ))
		{
			if ( i == ( (int)dirname.length() -1 ) )
			{
				tmp_dirname+=dirname[i];
			}
			//if
			// makedir
			// else return 1;
			if ( checkDirectory(tmp_dirname, true) != 0 )
			{
				if (makeDir(tmp_dirname) != 0)	
				{
					return 1;
				}
			}
			
			if (changeDirectory(tmp_dirname) != 0)
			{
				return 1;
			}
			tmp_dirname.clear();
		}
		else
		{
			tmp_dirname+=dirname[i];
		}
	}
	
	changeDirectory(back);
	return 0;
};
*/

int fmanager::removeDirectory(const string& dir)
{	
	string dirname = dir;

	dirname=getFullPath(dirname,true);
	
	DIR *directory;
	struct dirent *entry;
	//struct stat fstat;
	directory = opendir(dirname.c_str());
	
	if ( directory == NULL )
	{
		setError();
		return 1;
	}
	
	//chdirectory(dirname);
	
	while ( (entry=readdir(directory)) != NULL )
	{
		// if file remove else call remove_directory on it	
		if(0 == (strcmp(entry->d_name,".")))
		{
			continue;
		}
		else if(0 == (strcmp(entry->d_name,"..")))
		{
			continue;
		}
		else
		{
			//doStat((string)entry->d_name);
			//stat(entry->d_name,&fstat); // put if condition to check return status
			string entname = (dirname+(string)entry->d_name);
			if (checkFile(entname, false) == 0)
			{
				removeFile(entname);
			}
			else if (checkDirectory(entname, true) == 0)
			{	
				if ( rmdir(entname.c_str()) != 0 )
				{
					removeDirectory(entname);
				}
			}
			
		}
	}
	
	//chdirectory("..");
	rmdir(dirname.c_str());
	closedir(directory);
	return 0;
};




string fmanager::getFullPath(const string& gpath, const bool& dir)
{
	string rootstr;
	string path = gpath;
	rootstr.append(getCurrDir());
	rootstr.append(pathSeperator);
	// if first char is / or 2nd is : and 3rd are \\ then
	if (path.substr(0,2) == "./")
	{
		//path.replace(0,2,("/"+get_currdir()));
		path.erase(0,2);
		path.insert(0,rootstr);
	}
	else if ((path.at(0)=='/') || (path.substr(1,2) == ":\\"))
	{
		;
	}
	else if ( path.empty() )
	{
		return string();
	}
	else
	{
		//path.append(((const)get_currdir()+"/")); path.append(get_currdir());
		path.insert(0,rootstr);
	}
	
	if (dir)
	{
		if ( path.substr(path.size()-1) !=  pathSeperator)
		{
			path.append(pathSeperator);
		}
	}
	
	return path.c_str();
}



string fmanager::getTopPath(const string& path)
{
	int i=0;
	string topdir;
	
	for (i=0; i<(int)path.length(); i++) // not sure if this is fastest or just str::find("/\\")
	{
			if ( ( path.at(i) =='/' && i != 0 ) || ( path.at(i) =='\\' && i!= 2 ) )
			{
				topdir = path.substr(0,i+1);
				break;
			}
	}
	
	if(topdir.size() == 0)
	{
		topdir = path + pathSeperator;
	}
	return topdir.c_str();
};

string fmanager::getChildPath(const string& path)
{
	if( (path.at(path.size()-1) == '/') || (path.at(path.size()-1) == '\\') )
	{
		return path.substr(path.substr(0,path.size()-1).find_last_of("/\\")+1);
	}
	
	return path.substr(path.find_last_of("/\\")+1)+pathSeperator;
};

string fmanager::cleanPath(const string path)
{
	if ( path.substr(path.size()-1) == pathSeperator)
	{
		return path.substr(0,path.size()-1);
	}
	
	return path;
};

bool fmanager::isFullPath(const string& path)
{
	if ( (path.at(0)=='/') || (path.substr(1,2) == ":\\"))
	{
		return true;
	}
	
	return false;
};


int fmanager::makeDirectory(const string& dirname)
{
	string dir = cleanPath(dirname);
	/*if(dirname.find_last_of("/\\") == dirname.size()-1)
	{
		dir += pathSeperator;
	}*/
		
	if(dir.find_first_of("/\\") != string::npos)
	{
		size_t pos = 0;
		while(pos !=  string::npos)
		{
			string subdir = dir.substr(0, dir.find_first_of("/\\", pos));
			if(checkDirectory(subdir))
			{
				if ( mkdir(subdir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
				{
					return 1;
				}
			}
			else
			{
				return 1;
			}
			
			size_t l = dir.find_first_of("/\\", pos);
			if(l != string::npos)
			{
				pos = l+1;
			}
			else
			{
				pos = string::npos;
			}
		}
		
		return 0;
	}
	
	if (mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0)
	{
		return 0;
	}
	
	return 1;
};


int fmanager::copyDirectory(const string& gsrcdir, const string& gdestdir, const bool& overwrite)
{
	// we get /mnt/cool/crap, so we need that crp dir, dest is /mnt/notcool/alsocrap , we will creat the destdir then
	// loop through the sec dir , if file , then cpy it , if dir , then create it in destdir and chdir into both
	
	// first we create the dest dir if it doesnt exist , else return error ( if force isnt there )
	// then we lop through the src dir and copy files and create directories
	
	//string dirs;
	
	string destdir = getFullPath(gdestdir,true);
	string srcdir = getFullPath(gsrcdir,true);
	
	if ( checkDirectory(destdir, true) != 0 )
	{
		createDirectory(destdir);
	}
	
	DIR *directory;
	struct dirent *entry;
	directory = opendir(srcdir.c_str());
	//chdirectory(srcdir);
	if ( directory == NULL )
	{
		setError();
		//cout << "lol";
		return 1;
	}
	
	while ( (entry=readdir(directory)) != NULL )
	{

		// if file remove else call remove_directory on it	
		if(0 == (strcmp(entry->d_name,".")))
		{
			continue;
		}
		else if(0 == (strcmp(entry->d_name,"..")))
		{
			continue;
		}
		else
		{
			//stat(entry->d_name,&fstat); // put if condition to check return status
			if (checkFile((string)entry->d_name, false) == 0)
			{
				copyFile((string)entry->d_name,((destdir)+(string)entry->d_name),overwrite);
			}
			else if (checkDirectory((string)entry->d_name, true) == 0)
			{	
				if ( (entry->d_name) != NULL )
				{
					//create_directory((destdir)+pathSeperator+(string)entry->d_name);
					//dirs += entry->d_name;
					//dirs += "/";
					copyDirectory((srcdir)+(string)entry->d_name,(destdir)+(string)entry->d_name,overwrite);
				}
			}
			
		}
	}
	
	//chdirectory("..");
	//rmdir(dirname.c_str());
	//cout << dirs.find_last_of("/");
	//if (dirs.find_last_of("/"))
	//{
//		dirs.erase(dirs.find_last_of("/"));
//	}
	
	closedir(directory);
	
	
	// new string
	// when chdir to one , add it with /
	// when chdir.. , remove the last one
	
	return 0;
	
};


int fmanager::moveDirectory(const string& srcdir, const string& destdir, const bool& overwrite)
{
	if (!copyDirectory(srcdir, destdir,overwrite))
	{
		removeDirectory(srcdir);
	}
	else
	{
		return 1;
	}
	
	return 0;
};

string fmanager::getError()
{
	return error;
};

string fmanager::getFileData2(const string& fname, const unsigned long& size, const unsigned long& offset)
{
	//string data;
	//fname = get_fullpath(fname,false);
	
	if ( checkFile(fname, true) != 0 )
	{
		error = "Could not find file : " + fname;
		return NULL;
	}
	
	ifstream fs;
	fs.open(fname.c_str(),ios::in);
	if (!fs.is_open())
	{
		error = "Could not open file : " + fname;
		return "";
	}
	
	int len;
	
	if ( size == 0 )
	{
		fs.seekg(0,ios::end);
		len = fs.tellg();
		fs.seekg(0,ios::beg); // commentedfor better performance ?
	}
	else
	{
		len = size;
		//fs.seekg(0,ios::end);
	}
	
	if (offset)
	{
		fs.seekg(offset,ios::beg);
	}
	
	if ( len > SSIZE_MAX)
	{
		setError("File is too big to be read");
		return "";
	}
	
	char* buff  = new char[len];
	
	
	fs.read(buff,len);

	string data(buff,buff+len);
	
	fs.close();
	delete buff;
	return data;
};

string fmanager::getFileData(const string& fname)
{	
	if ( checkFile(fname, true) != 0 )
	{
		error = "Could not find file : " + fname;
		return (NULL);
	}
	
	ifstream fs;
	fs.open(fname.c_str(),ios::in);
	if (!fs.is_open())
	{
		error = "Could not open file : " + fname;
		return (NULL);
	}

	string data("");
	string tmpstr("");
	while (getline(fs,tmpstr,'\n'))
	{
		data += tmpstr;
	}
	
	fs.close();
	return (data);
};

int fmanager::resetStat()
{
	//if (Stat)
	//{
		//Stat = NULL;
		delete Stat;
		Stat = new struct stat;
		return 0;
	//}
	
	return 1;
};

vector<string>* fmanager::searchDirectory(const string& dir, const string& name, const bool& recursive, const bool& exact, const bool& skipPermission = true)
{
	resetStat();
	
	DIR *directory(opendir(dir.c_str()));
	struct dirent *entry;
	//directory = opendir(dir.c_str());
	if ( directory == NULL )
	{
		if ((skipPermission && (errno == EACCES)))
		{
			//return (new vector<string>);
			return(NULL);
		}
		else
		{
			setError();
			return(NULL);
		}
	}
	
	vector<string>* result = new vector<string>;
	
	while ( (entry=readdir(directory)) != NULL )
	{
		// if file remove else call remove_directory on it	
		if( ! (( (strcmp(entry->d_name,".")) == 0 ) || ( (strcmp(entry->d_name,"..")) == 0 )) )
		{
			string entname((static_cast<string>(dir))+pathSeperator+(static_cast<string>(entry->d_name)));
			string ent(static_cast<string>(entry->d_name));
			if ( checkFile(entname, false) == 0 )
			{
				if ( ent == name)
				{
					result->push_back(entname);
					//return ( (string)((string)((dir)+pathSeperator+((string)entry->d_name))) );
				}
			}
			else
			{
				// ops , its a trap :D , lets get out
				
				if (errno == ELOOP )
				{
					continue;
				}
				
				if ( checkDirectory(entname, true) == 0)
				{
					//if ( (entry->d_name) != NULL )
					//{
						if ( ent == name )
						{
							//return (((string)(get_fullpath(dir,true))+((string)entry->d_name)));
							if (exact)
							{
								result->push_back(entname);
								closedir(directory);
								return (result);
							}
							
							//return (((string)(dir)+pathSeperator+((string)entry->d_name)));
						}
						else
						{
							if (recursive)
							{
								//chdirectory(get_fullpath(dir,true));
								vector<string>* sresult;
								sresult = searchDirectory(entname, name, recursive, exact, skipPermission);
								
								//if (sresult == NULL)
								//{
								//	delete result;
								//	return (NULL);
								//}
								//else if (sresult)
								if (sresult)
								{
									if( sresult->size() != 0	)
									{
										for ( unsigned int i =0; i < sresult->size(); i++ )
										//for ( unsigned int i =sresult->size()-1; i > 0 ; --i )
										{
											result->push_back(sresult->at(i));
										}
										closedir(directory);
										return (result);
									}
								}
								
							}
						}
					//}
				}	
			}
		}
	}
	
//	chdirectory("..");
	
	closedir(directory);
	
	return (result);
};

vector<FINFO*>* fmanager::listDirectory(const string& dir, const bool& recursive,const bool& skipPermission, const string& date)
{
	//resetStat();
	
	if ( checkDirectory(dir, true) != 0 )
	{
		return NULL;
	}
	
	DIR *directory;
	struct dirent *entry;
	directory = opendir(dir.c_str());
	
	if ( directory == NULL )
	{
		setError();
		return NULL;
	}
	
	vector<FINFO*>* finfo = new vector<FINFO*>;
	
	while ( (entry=readdir(directory)) != NULL )
	{
		if( ! (( (strcmp(entry->d_name,".")) == 0 ) || ( (strcmp(entry->d_name,"..")) == 0 )) )
		{
			
			string entname((static_cast<string>(dir))+pathSeperator+(static_cast<string>(entry->d_name)));
			string ent(static_cast<string>(entry->d_name));
			
			FINFO* fent = getFileInfo(entname, date, true, true);
			
			// ops , its a trap :D
			if (errno == ELOOP )
			{
				continue;
			}
			
			if (fent->Type == DIRECTORY)
			{	
				if (recursive)
				{
					vector<FINFO*>* sfinfo;
					//sresult = search_directory(entname, name, recursive, exact, skipPermission);
					sfinfo = listDirectory(entname, recursive, skipPermission, date);
					if (sfinfo)
					{
						if( sfinfo->size() != 0	)
						{
							for ( unsigned int i =0; i < sfinfo->size(); i++ )
							{
								finfo->push_back(sfinfo->at(i));
							}
						}
					}		
				}
				
			}
			
			finfo->push_back(fent);
		}	
	}
	if( finfo->size() != 0	)
	{
		return finfo;
	}
	return NULL;
};

int fmanager::setError()
{
	switch (errno)
	{
		case EACCES: error="EACCES : Permission denied.(or part of the path...)";
				break;
		case EBADF:  error="EBADF : fd is not a valid file descriptor opened for reading.";
				break;
		case ENOENT: error="ENOENT : Directory/File does not exist, or name is an empty string.(or part of the path,,,)";
				break;
		case ENOMEM: error="ENOMEM : Insufficient memory to complete the operation.";
				break;
		case EMFILE: error="EMFILE : Too many file descriptors in use by process.";
				break;
		case ENFILE: error="ENFILE : Too many files are currently open in the system.";
				break;
		case ENOTDIR: error="ENOTDIR : name is not a directory (or part of the path).";
				 break;
	}
	
	return errno;
};

int fmanager::setError(const string& err)
{
	if ( err.size() )
	{
		error = err;
		return (0);
	}
	
	return (1);
};


#ifdef linux
int fmanager::Mount(const string& dev, const string& dest, const string& fsType="", int flags = NO_FLAGS_AND_DATA )
{
	return mount(dev.c_str(), dest.c_str(), fsType.c_str(), flags, "");
}

int fmanager::unMount(const string& dev, bool Force = false)
{
	return umount2(dev.c_str(), (int)Force);
}
#endif
