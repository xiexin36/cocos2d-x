#pragma once

class GAFTextureAtlas;
class GAFSubobjectState;

class GAFAnimationFrame
{
public:
    typedef std::vector<GAFSubobjectState*> SubobjectStates_t;
private:
    SubobjectStates_t       m_subObjectStates;
public:
    GAFAnimationFrame();
    ~GAFAnimationFrame();
    const SubobjectStates_t& getObjectStates() const;

    void    pushObjectState(GAFSubobjectState*);
};
