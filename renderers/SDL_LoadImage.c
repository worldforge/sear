#ifdef __APPLE__

#include <SDL/SDL.h>

#include <Carbon/Carbon.h>
#include <QuickTime/ImageCompression.h> // for image loading and decompression
//#include <QuickTime/QuickTimeComponents.h> // for file type support

SDL_Surface* IMG_Load(const char* fileName)
{
        GWorldPtr pGWorld = NULL;
    OSStatus err = noErr; // err return value
    SDL_Surface* surface = NULL;
    
    FSSpec imgSpec;
    FSRef	ref;

    /* convert the POSIX path to an FSRef */
    err = FSPathMakeRef(fileName, &ref, false); // fname is not a directory
    if (err != noErr) return NULL;
	
    /* and then convert the FSRef to an FSSpec */
    err = FSGetCatalogInfo(&ref, kFSCatInfoNone, NULL, NULL, &imgSpec, NULL);
    if (err != noErr) return NULL;

    // get imprter for the image type in file
    GraphicsImportComponent giComp; 
    err = GetGraphicsImporterForFile (&imgSpec, &giComp);
    if (err != noErr) { // if we have an error
        std::cout << "unable to locate image importer for " << fileName << std::endl;
        return NULL; // go away
    }

    // Create GWorld
    Rect rectImage; // rectangle of source image
    GraphicsImportGetNaturalBounds (giComp, &rectImage); // get the image bounds
    int imgWidth = rectImage.right - rectImage.left,
        imgHeight = rectImage.bottom - rectImage.top;
    
    OSType pixelFormat = k32ARGBPixelFormat;
    int depth = 32;
    
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, imgWidth, imgHeight, depth,
        0xff000000, 0xff0000, 0xff00, 0xff);
    assert(surface);
    SDL_LockSurface(surface);
        
    err = QTNewGWorldFromPtr (&pGWorld, pixelFormat, &rectImage, NULL, NULL, 0, 
        surface->pixels, surface->pitch); 
   if (err != noErr) {
        std::cout << "error " << err << " creating GWorld of size (" << imgWidth << "," <<
            imgHeight << ")" << std::endl;
        return NULL;
    }
    
    GDHandle origDevice; // save field for current graphics device
    CGrafPtr origPort; // save field for current graphics port
    GetGWorld (&origPort, &origDevice); // save onscreen graphics port
    
    if (err == noErr)
        err = GraphicsImportSetGWorld (giComp, pGWorld, NULL); // set the destination of the importer component
    if (err == noErr)
        err = GraphicsImportSetQuality (giComp, codecLosslessQuality); // we want lossless decompression
    if ((err == noErr) && GetGWorldPixMap (pGWorld) && LockPixels (GetGWorldPixMap (pGWorld)))
        GraphicsImportDraw (giComp); // if everything looks good draw image to locked pixmap
    else {
        std::cout << "error drawing image with graphics importer" << std::endl;
        return NULL;
    }

    UnlockPixels (GetGWorldPixMap (pGWorld)); // unlock pixels
    CloseComponent(giComp); // dump component
    SetGWorld(origPort, origDevice); // set current graphics port to offscreen
    DisposeGWorld (pGWorld); // dump gworld
    
    for (unsigned int y=0; y < imgHeight; ++y) {
        Uint32* pixel = surface->pixels + (y * surface->pitch);
        for (unsigned int x=0; x < imgWidth; ++x)
        {
            *pixel = (*pixel << 8) | (*pixel >> 24);
            ++pixel; 
        }
    }
    
    SDL_UnlockSurface(surface);    
    return surface;}

#endif

