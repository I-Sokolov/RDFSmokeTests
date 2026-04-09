typedef std::map<ExpressID, ExpressID> InstanceMap;

extern void ReadWriteDataFileTest();
extern void ReadWriteSchemaTest();
extern void ModelCheckerTests();

extern void SaveModelByAPI(SdaiModel model, const char* filePath);

extern SdaiModel CopyModelByAPI(SdaiModel model, InstanceMap& instanceMap);
