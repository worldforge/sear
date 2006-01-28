#ifndef SEAR_RENDER_SPRITE_H
#define SEAR_RENDER_SPRITE_H

#include <varconf/Config.h>
#include <sage/sage.h>
#include <sage/GL.h>

namespace Sear
{

class Render;
class SpriteData;

class Sprite
{
public:
    Sprite() : m_data(NULL) { ; }
    
    Sprite(const std::string& name);
    ~Sprite();
    
    Sprite(const Sprite& other);
    Sprite& operator=(const Sprite& other);
    
    void draw(Render* r);
private:
    SpriteData* m_data;
};

/** the sprite implementation class. If multiple renderer are ever
 supported, this should become an abstract base, with derived versions
 of GLSpriteData, DX9SpriteData and so on.
 
 SpriteDatas are ref-counted.
*/
class SpriteData
{
public:
    SpriteData(const std::string& name);
    ~SpriteData();
    
    void draw(Render* render);
    
    void contextCreated();
    void contextDestroyed(bool check);
    
private:
    friend class Sprite;
    
    static varconf::Config& getSpriteConfig();
    
    void load();
    /** helper method invoked when the load fails for some reason */
    void loadFail();
    
    unsigned int twoN(unsigned int size);

    void addRef()
    { ++m_refCount; }
    
    void decRef();
    
    std::string m_name;
    unsigned int m_refCount;
    bool m_valid;
    
    int m_width, m_height;
    GLuint m_textureId;
    float m_pw, m_ph;
};

} // of namespace Sear

#endif // of #ifndef SEAR_RENDER_SPRITE_H
