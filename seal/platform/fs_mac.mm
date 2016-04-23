#import <Foundation/Foundation.h>

extern "C" {
    
static const char* write_path = NULL;
const char* get_write_path_mac() {
    if (!write_path) {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths objectAtIndex:0];
        write_path = [documentsDirectory UTF8String];
    }
    return write_path;
}
    
}