
#include <iostream>
#include <fstream>
#include <cstdlib>

int main(int argc, char *argv[])
{
    std::cout << "create-git-proj\n";
    std::cout << "using:\n";
    std::cout << "./create-git-proj project.git\n";
    
    std::string projName = argv[1];
    std::string path = "/Library/WebServer/Documents/repo";
    std::string tmp;

    // create folder
    tmp = std::string("sudo mkdir ") + path + std::string("/") + projName;
    system(tmp.c_str());

    // sudo git init --bare
    tmp = std::string("sudo git init --bare ") + path + std::string("/") + projName;
    system(tmp.c_str());

    // change the name
    tmp = std::string("sudo mv ") + path + std::string("/") + projName + std::string("/hooks/post-update.sample ") + 
          path + std::string("/") + projName + std::string("/hooks/post-update ");
    system(tmp.c_str());

    // change the owner
    tmp = std::string("sudo chown -R _www:_www ") + path + std::string("/") + projName;
    system(tmp.c_str());

    // create a bash script for sudo -u _www
    std::ofstream ftmp("a.sh");
    ftmp << "#!/bin/bash" << std::endl;

    tmp = std::string("cd ") + path + std::string("/") + projName;
    ftmp << tmp.c_str() << std::endl;
    
    tmp = std::string("sudo -u _www ") + std::string("hooks/post-update");
    ftmp << tmp << std::endl;

    ftmp.close();

    // give the right to run the script
    system("chmod +x a.sh");

    // run the script
    system("./a.sh");

    // delete the script
    system("rm a.sh");

    std::ofstream fgroup("/etc/apache2/other/htgroup-git", std::ios::app | std::ios::ate);
    if (!fgroup.is_open()) {
        tmp = "sudo rm -r " + path + std::string("/") + projName;
        system(tmp.c_str());
        std::cerr << "Error /etc/apache2/other/htgroup-git file not found!" << std::endl;
    }

    fgroup << std::endl;
    fgroup << projName.substr(0, projName.size() - 4) << "-reader: readwrite" << std::endl;
    fgroup << projName.substr(0, projName.size() - 4) << "-writer: readwrite" << std::endl;

    fgroup.close();

    std::ofstream fgitconf("/etc/apache2/other/gitrepo.conf", std::ios::app | std::ios::ate);
    
    fgitconf << std::endl;
    fgitconf << "<Directory " << path << "/" << projName << ">" << std::endl;
    fgitconf << "    Allow from all" << std::endl;    
    fgitconf << "    Order allow,deny" << std::endl;
    fgitconf << "    <Limit GET>" << std::endl;
    fgitconf << "         Require group " << projName.substr(0, projName.size() - 4) << "-reader" << std::endl;
    fgitconf << "    </Limit>" << std::endl;
    fgitconf << "    <Limit GET PUT POST DELETE PROPPATCH MKCOL COPY MOVE LOCK UNLOCK>" << std::endl;
    fgitconf << "         Require group " << projName.substr(0, projName.size() - 4) << "-writer" << std::endl;
    fgitconf << "    </Limit>" << std::endl;
    fgitconf << "</Directory>" << std::endl;

    fgitconf.close();

    system("sudo apachectl restart");

    return 0;
}
