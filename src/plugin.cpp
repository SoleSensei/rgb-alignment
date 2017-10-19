#include <assert.h>

#include "mvc.h"

string Plugin::get_name()
{
    return name;
}

Library::Library(Modeler* m): names(), libs(), plugs(), loaded(0), model(m)
{
    for(uint i = 0; i < MAX_NUM_LIBS; ++i)
        plugs[i] = NULL;
}

int Library::find_libs(){
    DIR *dp;
    struct dirent *dirp;

    if((dp  = opendir("./plugins")) == NULL) {
    	model->buf += "Error: can't open ./plugins \n";
        return 1;
    }
    int k = 0;
    while ((dirp = readdir(dp)) != NULL) {
        char *dot = strrchr(dirp->d_name, '.'); 
        if (dot && (strcmp(dot, ".so") == 0))
        {
            libs[k] = string(dirp->d_name);
            cout << libs[k++];
        }
    }    
    if(closedir(dp) < 0)
        return 1;
    loaded = k;
    return 0;
}

int Library::load_libs()
{
    find_libs(); 
    uint k = 0;
    void* handle;
    for (int i=0; i<loaded; ++i){
        string name = libs[i];        
        string pathname = "./plugins/" + name;
        handle = dlopen(pathname.c_str(), RTLD_LAZY);
        if (handle){
            plugs[k] = handle;
            name = name.substr(4,name.size()-7);
            names[k++] = name;
        }
    }

    loaded = k;
    return loaded;
}

void Library::print_loaded()
{
    model->buf += std::to_string(loaded) +  " filters loaded \n";
    for(int i = 0; i < loaded; ++i){
        model->buf += "[" + std::to_string(i+1) + "] " + names[i] + "\n";
    } 
}

string Library::choosen_filter(int num)
{
    if (num>0 && num <= loaded)
        return names[num-1];
    else return "align";
}

Image Library::do_plugin(Image src, string& name, const int& radius, double fraction)
{   
    int num = -1;
    for(uint i = 0; i < MAX_NUM_LIBS; ++i){
        if(name == names[i]){
            num = i;
            break;
        }
    }
    if (num == -1){
        model->buf += "Error: filter [" + name + "] not founded \n";
        return src;
    }
    void* handle = plugs[num];

    Plugin* (*create)();
    void (*destroy)(Plugin*);

    create = reinterpret_cast<Plugin* (*)()>( dlsym(handle, "create_object") );
    destroy = reinterpret_cast<void (*)(Plugin*)>( dlsym(handle, "destroy_object") );
    Plugin* plugin = reinterpret_cast<Plugin*>( create() );
    Image dst = plugin->processing(src, radius, fraction);
    destroy( plugin );

    return dst;
}