// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef COMMON_OPERATIONS_H
#define COMMON_OPERATIONS_H

#include <Atlas/Objects/Operation.h>

using Atlas::Objects::Operation::SetData;

class WieldData;
typedef Atlas::Objects::SmartPtr<WieldData> Wield;

class UseData;
typedef Atlas::Objects::SmartPtr<UseData> Use;

static int USE_NO = -1;
static int WIELD_NO = -1;

class UseData : public SetData
{
protected:
    /// Construct a UseData class definition.
    UseData(UseData *defaults = NULL) : SetData((SetData*)defaults)
    {
        m_class_no = USE_NO;
    }
    /// Default destructor.
    virtual ~UseData();

public:
    /// Copy this object.
    virtual UseData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == USE_NO) current_class = -1; SetData::iterate(current_class, attr);}

    //freelist related things
public:
    static UseData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual UseData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static UseData *getDefaultObjectInstance();
private:
    static UseData *defaults_UseData;
    static UseData *begin_UseData;
};


class WieldData : public SetData
{
protected:
    /// Construct a WieldData class definition.
    WieldData(WieldData *defaults = NULL) : SetData((SetData*)defaults)
    {
        m_class_no = WIELD_NO;
    }
    /// Default destructor.
    virtual ~WieldData();

public:
    /// Copy this object.
    virtual WieldData * copy() const;

    /// Is this instance of some class?
    virtual bool instanceOf(int classNo) const;


    virtual void iterate(int& current_class, std::string& attr) const
        {if(current_class == WIELD_NO) current_class = -1; SetData::iterate(current_class, attr);}

    //freelist related things
public:
    static WieldData *alloc();
    virtual void free();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of the same class as this object.
    ///
    /// @return a pointer to the default object.
    virtual WieldData *getDefaultObject();

    /// \brief Get the reference object that contains the default values for
    /// attributes of instances of this class.
    ///
    /// @return a pointer to the default object.
    static WieldData *getDefaultObjectInstance();
private:
    static WieldData *defaults_WieldData;
    static WieldData *begin_WieldData;
};
#endif
