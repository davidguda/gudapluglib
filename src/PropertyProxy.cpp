/*
  ==============================================================================

    PropertyProxy.cpp
    Created: 22 Apr 2014 7:54:48am
    Author:  David Gunnarsson

  ==============================================================================
*/

#include "PropertyProxy.h"
#include "debug.h"

juce::CriticalSection globalPropertyProxyLock;
static PropertiesFile* staticProperties = 0;
static int proxys = 0;

PropertyProxy::PropertyProxy() {
    const ScopedLock sl (globalPropertyProxyLock);
    properties = nullptr;
    
    if(staticProperties && proxys == 0) {
        DBUG(("something is wrong here!"));
    }
    
    if(!staticProperties) {
        staticProperties = new PropertiesFile(getPropertyOptions());
        if(proxys != 0) {
            DBUG(("WARNING, %i proxys but still no staticProperties", proxys));
        }
        proxys = 0;
    }
    
    properties = staticProperties;
    
    proxys++;
}

PropertyProxy::~PropertyProxy() {
    const ScopedLock sl (globalPropertyProxyLock);
    proxys--;
    if(proxys == 0) {
        if(staticProperties) {
            delete staticProperties;
            staticProperties = 0;
        } else {
            DBUG(("WARNING: properties is empty"));
        }
    } else if(proxys < 0) {
        //DBUG(("WARNING, proxys %i", proxys));
    }
}

PropertiesFile::Options PropertyProxy::getPropertyOptions() {
    PropertiesFile::Options propertyOptions;
    propertyOptions.applicationName = "DrumR";
    static InterProcessLock propertyFileLock("propertyFileLock");
    propertyOptions.processLock = &propertyFileLock;
    propertyOptions.commonToAllUsers = false;
    propertyOptions.osxLibrarySubFolder = "Application Support";
    propertyOptions.folderName = "DrumR";
    propertyOptions.filenameSuffix = ".settings";
    return propertyOptions;
}

const File& PropertyProxy::getFile() {
    return properties->getFile();
}
void PropertyProxy::changeListenerCallback (ChangeBroadcaster* source) {
    DBGF;
    sendChangeMessage();
}

int PropertyProxy::getIntValue (StringRef keyName, int defaultReturnValue) {
    const ScopedLock sl (globalPropertyProxyLock);
    if(properties) {
        return properties->getIntValue(keyName, defaultReturnValue);
    } else {
        DBUG(("WARNING, bad properties"));
        return 0;
    }
}

bool PropertyProxy::getBoolValue (StringRef keyName, int defaultReturnValue) {
    return getIntValue(keyName, defaultReturnValue) != 0;
}

void PropertyProxy::setValue (const String& keyName, const var& value) {
    const ScopedLock sl (globalPropertyProxyLock);
    if(properties) {
        properties->setValue(keyName, value);
        properties->saveIfNeeded();
    } else {
        DBUG(("WARNING, bad properties"));
    }
}

String PropertyProxy::getValue (StringRef keyName, const String& defaultReturnValue) {
    const ScopedLock sl (globalPropertyProxyLock);
    if(properties) {
        return properties->getValue(keyName, defaultReturnValue);
    } else {
        DBUG(("WARNING, bad properties"));
        return "";
    }
}

bool PropertyProxy::containsKey(StringRef key) {
    const ScopedLock sl (globalPropertyProxyLock);
    if(properties) {
        return properties->containsKey(key);
    } else {
        DBUG(("WARNING, bad properties"));
        return false;
    }
}

bool PropertyProxy::reload() {
    const ScopedLock sl (globalPropertyProxyLock);
    if(properties) {
        return properties->reload();
    } else {
        DBUG(("WARNING, bad properties"));
    }
    return false;
}

bool PropertyProxy::saveIfNeeded() {
    const ScopedLock sl (globalPropertyProxyLock);
    if(properties) {
        return properties->saveIfNeeded();
    } else {
        DBUG(("WARNING, bad properties"));
    }
    return false;
}

bool PropertyProxy::save() {
    const ScopedLock sl (globalPropertyProxyLock);
    if(properties) {
        return properties->save();
    } else {
        DBUG(("WARNING, bad properties"));
    }
    return false;
}











