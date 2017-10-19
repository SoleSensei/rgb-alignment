#include <dlfcn.h>
#include <assert.h>

#include "mvc.h"

string Plugin::get_name()
{
    return name;
}

Library::Library(Modeler* m): names(), plugs(), loaded(0), model(m)
{
    for(uint i = 0; i < MAX_NUM_LIBS; ++i)
        plugs[i] = NULL;
}

int Library::load_libs()
{
    // string file = 
    uint k = 0;
    void* handle;
    handle = dlopen("./plugins/lib_gw.so", RTLD_LAZY);
    if (handle){
        plugs[k] = handle;
        names[k++] = "gray-world";
    }
    handle = dlopen("./plugins/lib_median.so", RTLD_LAZY);
    if (handle){
        plugs[k] = handle;
        names[k++] = "median";
    }        
    handle = dlopen("./plugins/lib_unsharp.so", RTLD_LAZY);
    if (handle){
        plugs[k] = handle;
        names[k++] = "unsharp";
    }   
    handle = dlopen("./plugins/lib_autoc.so", RTLD_LAZY);
    if (handle){
        plugs[k] = handle;
        names[k++] = "autocontrast";
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