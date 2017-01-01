/*
  ==============================================================================

    PropertyProxy.h
    Created: 22 Apr 2014 7:54:48am
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "globalStuff.h"

#ifndef PROPERTYPROXY_H_INCLUDED
#define PROPERTYPROXY_H_INCLUDED

class PropertyProxy: public ChangeBroadcaster, ChangeListener
{
public:
    PropertyProxy(string name_in = "DrumR");
    ~PropertyProxy();
    
    virtual void changeListenerCallback (ChangeBroadcaster* source);
    
    int getIntValue (StringRef keyName, int defaultReturnValue = 0);
    bool getBoolValue (StringRef keyName, int defaultReturnValue = 0);
    void setValue (const String& keyName, const var& value);
    String getValue (StringRef keyName, const String& defaultReturnValue = String());
    bool containsKey(StringRef key);
    bool reload();
    bool saveIfNeeded();
    bool save();
    const File& getFile();
private:
    string name;
    PropertiesFile* properties = nullptr;
    PropertiesFile::Options getPropertyOptions(string name);
};



#endif  // PROPERTYPROXY_H_INCLUDED
