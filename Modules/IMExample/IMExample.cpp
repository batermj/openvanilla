// OVIMExample.cpp: a simple compose-and-commit input method
// Copyright (c) 2004-2005 The OpenVanilla Project (http://openvanilla.org)

#include <OpenVanilla/OpenVanilla.h>
#include <OpenVanilla/OVModuleWrapper.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

const int maxComposingStringLength=36;

class ExampleComposingString
{
public:
    ExampleComposingString() { len=0; }
    void remove() { if (len) composeStr[--len]=0; }
    const char *getString() { return composeStr; }
    void add(char c)
    {
        if (len < maxComposingStringLength) composeStr[len++]=c;
        composeStr[len]=0;
    }
    
protected:
    int len;
    char composeStr[maxComposingStringLength];
};

class OVIMExampleContext : public OVInputMethodContext
{
public:
    virtual int keyEvent(OVKeyCode *key, OVBuffer *buf, OVTextBar *textbar,
        OVService *srv)
    {
        // if there's something in buffer, we commit
        if (key->code()==ovkReturn && buf->length())
        {
            buf->send();
            textbar->clear()->hide();
            return 1;
        }
        
        // process backspace
        if (key->code()==ovkBackspace && buf->length())
        {
            str.remove();
            buf->clear()->update();            
            if (strlen(str.getString()))
            {
                buf->append(str.getString())->update();
                textbar->clear()->append("composing: ")->append(str.getString())->
                    update()->show();
            }
            else
            {
                textbar->clear()->hide();
            }
            
            return 1;
        }
        
        // if it's printable character, we append both the pre-edit buffer
        // and textbar
        if (isprint(key->code()))
        {
            char b[2];
            sprintf(b, "%c", key->code());
            buf->clear()->append(b)->update();
            textbar->clear()->append("composing: ")->append(b)->update()->show();
            return 1;
        }
        
        // otherwise, we dump everything, clean up the buffer, hide the textbar
        // and return 0
        buf->send();
        textbar->clear()->hide();
        return 0;
    }
    
protected:
    ExampleComposingString str;
};

class OVIMExample : public OVInputMethod
{
public:
    virtual const char* identifier() { return "OVIMExample-Simple"; }
    virtual OVInputMethodContext *newContext() { return new OVIMExampleContext; }
};

OVMODULEWRAPPER_INPUTMETHOD(OVIMExample);
