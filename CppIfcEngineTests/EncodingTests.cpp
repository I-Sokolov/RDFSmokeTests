
#include "pch.h"

#define REG_CHARS_FILE_NAME "PutGetRegionalChars.ifc"


static const char* ANSI_STRING = "'English'\\ Русский'";
static const wchar_t* WCHAR_STRING = L"'English'\\ Русский'";
static const char* ANSI_STEP = R"(''English''\\ \X\D0\X\F3\X\F1\X\F1\X\EA\X\E8\X\E9'')";
static const char* UNICODE_STEP = R"(\X2\0027\X0\English\X2\0027\X0\\\ \X2\0420\X0\\X2\0443\X0\\X2\0441\X0\\X2\0441\X0\\X2\043A\X0\\X2\0438\X0\\X2\0439\X0\\X2\0027\X0\)";

static const char* ANSI_SLASH = "\\";
static const wchar_t* WCHAR_SLASH = L"\\";
static const char* STEP_SLASH = "\\\\";


static void CheckAttr(SdaiInstance inst, const char* attr, const char* ansi, const wchar_t* unicode, const char* step)
{
    char* getStr = NULL;
    wchar_t* getWStr = NULL;

    auto ret = sdaiGetAttrBN(inst, attr, sdaiSTRING, &getStr);
    ASSERT(ret && !strcmp(getStr, ansi));

    ret = sdaiGetAttrBN(inst, attr, sdaiUNICODE, &getWStr);
    ASSERT(ret && !wcscmp(getWStr, unicode));

    ret = sdaiGetAttrBN(inst, attr, sdaiEXPRESSSTRING, &getStr);
    ASSERT(ret && !strcmp(getStr, step));
}

static void CheckHeader(SdaiModel ifcModel, int_t subitem, const char* ansi, const wchar_t* unicode, const char* step)
{
    const char* str = NULL;
    GetSPFFHeaderItem(ifcModel, 0, subitem, sdaiSTRING, &str);
    ASSERT(!strcmp(str, ansi));

    const wchar_t* wstr = NULL;
    GetSPFFHeaderItem(ifcModel, 0, subitem, sdaiUNICODE, &wstr);
    ASSERT(!wcscmp(wstr, unicode));

    str = NULL;
    GetSPFFHeaderItem(ifcModel, 0, subitem, sdaiEXPRESSSTRING, &str);
    ASSERT(!strcmp(str, step));

}

static void CheckHeader(SdaiModel ifcModel)
{
    CheckHeader(ifcModel, 0, ANSI_STRING, WCHAR_STRING, ANSI_STEP);
    CheckHeader(ifcModel, 1, ANSI_STRING, WCHAR_STRING, UNICODE_STEP);
    CheckHeader(ifcModel, 2, ANSI_SLASH, WCHAR_SLASH, STEP_SLASH);
    CheckHeader(ifcModel, 3, ANSI_SLASH, WCHAR_SLASH, STEP_SLASH);
}


static void EncodingAndFilter(SdaiModel model)
{
    struct Codepage
    {
        enum_code_page code;
        int64_t        bitflag;
    };

    Codepage codepages[] = {
                                                                                        //		14, 15, 16, 17, 18, 19
        {enum_code_page::IGNORE_DEFAULT			    , (0 + 0 * 2 + 0 * 4 + 0 * 8 + 0 * 16 + 0 * 32)},	//		 0   0   0   0   0   0	
        {enum_code_page::WINDOWS_1250				, (0 + 0 * 2 + 1 * 4 + 0 * 8 + 0 * 16 + 0 * 32)},	//		 0   0   1   0   0   0	
        {enum_code_page::WINDOWS_1251				, (0 + 0 * 2 + 0 * 4 + 1 * 8 + 0 * 16 + 0 * 32)},	//		 0   0   0   1   0   0	
        {enum_code_page::WINDOWS_1252				, (0 + 0 * 2 + 1 * 4 + 1 * 8 + 0 * 16 + 0 * 32)},	//		 0   0   1   1   0   0	
        {enum_code_page::WINDOWS_1253				, (0 + 0 * 2 + 0 * 4 + 0 * 8 + 1 * 16 + 0 * 32)},	//		 0   0   0   0   1   0	
        {enum_code_page::WINDOWS_1254				, (0 + 0 * 2 + 1 * 4 + 0 * 8 + 1 * 16 + 0 * 32)},	//		 0   0   1   0   1   0	
        {enum_code_page::WINDOWS_1255				, (0 + 0 * 2 + 0 * 4 + 1 * 8 + 1 * 16 + 0 * 32)},	//		 0   0   0   1   1   0	
        {enum_code_page::WINDOWS_1256				, (0 + 0 * 2 + 1 * 4 + 1 * 8 + 1 * 16 + 0 * 32)},	//		 0   0   1   1   1   0	
        {enum_code_page::WINDOWS_1257				, (0 + 0 * 2 + 0 * 4 + 0 * 8 + 0 * 16 + 1 * 32)},	//		 0   0   0   0   0   1	
        {enum_code_page::WINDOWS_1258				, (0 + 0 * 2 + 1 * 4 + 0 * 8 + 0 * 16 + 1 * 32)},	//		 0   0   1   0   0   1	
        {enum_code_page::ISO8859_1					, (1 + 0 * 2 + 0 * 4 + 0 * 8 + 0 * 16 + 0 * 32)},	//		 1   0   0   0   0   0	
        {enum_code_page::ISO8859_2					, (1 + 0 * 2 + 1 * 4 + 0 * 8 + 0 * 16 + 0 * 32)},	//		 1   0   1   0   0   0	
        {enum_code_page::ISO8859_3					, (1 + 0 * 2 + 0 * 4 + 1 * 8 + 0 * 16 + 0 * 32)},	//		 1   0   0   1   0   0	
        {enum_code_page::ISO8859_4					, (1 + 0 * 2 + 1 * 4 + 1 * 8 + 0 * 16 + 0 * 32)},	//		 1   0   1   1   0   0	
        {enum_code_page::ISO8859_5					, (1 + 0 * 2 + 0 * 4 + 0 * 8 + 1 * 16 + 0 * 32)},	//		 1   0   0   0   1   0	
        {enum_code_page::ISO8859_6					, (1 + 0 * 2 + 1 * 4 + 0 * 8 + 1 * 16 + 0 * 32)},	//		 1   0   1   0   1   0	
        {enum_code_page::ISO8859_7					, (1 + 0 * 2 + 0 * 4 + 1 * 8 + 1 * 16 + 0 * 32)},	//		 1   0   0   1   1   0	
        {enum_code_page::ISO8859_8					, (1 + 0 * 2 + 1 * 4 + 1 * 8 + 1 * 16 + 0 * 32)},	//		 1   0   1   1   1   0	
        {enum_code_page::ISO8859_9					, (1 + 0 * 2 + 0 * 4 + 0 * 8 + 0 * 16 + 1 * 32)},	//		 1   0   0   0   0   1	
        {enum_code_page::ISO8859_10				    , (1 + 0 * 2 + 1 * 4 + 0 * 8 + 0 * 16 + 1 * 32)},	//		 1   0   1   0   0   1	
        {enum_code_page::ISO8859_11				    , (1 + 0 * 2 + 0 * 4 + 1 * 8 + 0 * 16 + 1 * 32)},	//		 1   0   0   1   0   1	
        {enum_code_page::ISO8859_13				    , (1 + 0 * 2 + 0 * 4 + 0 * 8 + 1 * 16 + 1 * 32)},	//		 1   0   0   0   1   1	
        {enum_code_page::ISO8859_14				    , (1 + 0 * 2 + 1 * 4 + 0 * 8 + 1 * 16 + 1 * 32)},	//		 1   0   1   0   1   1	
        {enum_code_page::ISO8859_15				    , (1 + 0 * 2 + 0 * 4 + 1 * 8 + 1 * 16 + 1 * 32)},	//		 1   0   0   1   1   1	
        {enum_code_page::ISO8859_16				    , (1 + 0 * 2 + 1 * 4 + 1 * 8 + 1 * 16 + 1 * 32)},	//		 1   0   1   1   1   1	
        {enum_code_page::MACINTOSH_CENTRAL_EUROPEAN , (0 + 1 * 2 + 0 * 4 + 0 * 8 + 0 * 16 + 0 * 32)},	//		 0   1   0   0   0   0	
        {enum_code_page::SHIFT_JIS_X_213			, (1 + 1 * 2 + 0 * 4 + 0 * 8 + 0 * 16 + 0 * 32)}	//		 1   1   0   0   0   0	
    };
   //not implemented  /*ISO8859_12				*/ (1 + 0 * 2 + 1 * 4 + 1 * 8 + 0 * 16 + 1 * 32),	//		 1   0   1   1   0   1	

    int64_t encoding_mask = (0b111111) << 14;

    //
    // all
    //

    for (int i = 0; i < _countof(codepages); i++) {

        int64_t setfilter = codepages[i].bitflag;
        setfilter = setfilter << 14;

        setFilter(model, setfilter, encoding_mask);

        auto getfilter = getFilter(model, encoding_mask);
        ASSERT(setfilter == getfilter);
    }

    for (int i = 0; i < _countof(codepages); i++) {

        auto code = codepages[i].code;

        engiSetAnsiStringEncoding(model, code);

        auto getfilter = getFilter(model, encoding_mask);
        ASSERT((codepages[i].bitflag << 14) == getfilter);
    }    

    //
    // switch
    //

    int64_t filter_ISO8859_6 = (1 + 0 * 2 + 1 * 4 + 0 * 8 + 1 * 16 + 0 * 32); //		 1   0   1   0   1   0	
    int64_t filter_ISO8859_7 = (1 + 0 * 2 + 0 * 4 + 1 * 8 + 1 * 16 + 0 * 32); //		 1   0   0   1   1   0	

    filter_ISO8859_6 <<= 14;
    filter_ISO8859_7 <<= 14;

    setFilter(model, filter_ISO8859_6, encoding_mask);
    auto getfilter = getFilter(model, encoding_mask);
    ASSERT(filter_ISO8859_6 == getfilter);

    int64_t change_mask = 0b11;
    change_mask <<= 16;
    
    getfilter = getFilter(model, change_mask);
    ASSERT(getfilter == ((int64_t)(0b01)<<16));

    int64_t bit17 = 1;
    bit17 <<= 17;

    setFilter(model, bit17, change_mask);

    getfilter = getFilter(model, encoding_mask);
    ASSERT(filter_ISO8859_7 == getfilter);

    getfilter = getFilter(model, change_mask);
    ASSERT(getfilter == ((int64_t)(0b10) << 16));
}

static void EncodingAndFilter()
{
    SdaiModel  model = sdaiCreateModelBNUnicode(L"IFC4");
    ASSERT(model);

    EncodingAndFilter(model);

    sdaiCloseModel(model);

    EncodingAndFilter(NULL);
}

static void CheckRegionalChars(SdaiModel ifcModel, SdaiInteger stepId)
{
    CheckHeader(ifcModel);

    auto wall = internalGetInstanceFromP21Line(ifcModel, stepId);

    CheckAttr(wall, "Name", ANSI_STRING, WCHAR_STRING, ANSI_STEP);
    CheckAttr(wall, "Description", ANSI_STRING, WCHAR_STRING, UNICODE_STEP);
    CheckAttr(wall, "ObjectType", ANSI_SLASH, WCHAR_SLASH, STEP_SLASH);
    CheckAttr(wall, "GlobalId", ANSI_SLASH, WCHAR_SLASH, STEP_SLASH);
}

//
const int_t BLOCK_LENGTH_READ = 20000;  //  MAX: 65535
FILE* myFileRead = nullptr;
static int_t   __stdcall   ReadCallBackFunction(unsigned char* content)
{
    if (myFileRead == nullptr || feof(myFileRead)) {
        return  -1;
    }

    int_t   size = fread(content, 1, BLOCK_LENGTH_READ, myFileRead);

    return  size;
}

static void CheckRegionalChars(const char* stepFile, SdaiInteger stepId)
{
    engiSetAnsiStringEncoding(NULL, enum_code_page::WINDOWS_1251);

    auto ifcModel = sdaiOpenModelBN(0, stepFile, "IFC4");
    CheckRegionalChars(ifcModel, stepId);
    sdaiCloseModel(ifcModel);

    fopen_s(&myFileRead, stepFile, "rb");
    ifcModel = engiOpenModelByStream(0, &ReadCallBackFunction, "IFC4");
    fclose(myFileRead);
    CheckRegionalChars(ifcModel, stepId);
    sdaiCloseModel(ifcModel);

    unsigned char content[10240];
    int_t size = _countof(content);
    fopen_s(&myFileRead, stepFile, "rb");
    size = fread(content, 1, size, myFileRead);
    fclose(myFileRead);

    ifcModel = engiOpenModelByArray(0, content, size, "IFC4");
    CheckRegionalChars(ifcModel, stepId);
    sdaiCloseModel(ifcModel);
}

//
const int_t BLOCK_LENGTH_WRITE = 20000; //  no maximum limit
FILE* myFileWrite = nullptr;
static void    __stdcall   WriteCallBackFunction(unsigned char* content, int64_t size)
{
    fwrite(content, (size_t)size, 1, myFileWrite);
}


static void PutGetRegionalChars(void)
{
    SdaiModel  ifcModel = sdaiCreateModelBNUnicode(L"IFC4");
    ASSERT(ifcModel);

    setFilter(ifcModel, 131072, ((int64_t)0b111111)<<14);

    SetSPFFHeaderItem(ifcModel, 0, 0, sdaiSTRING, ANSI_STRING);
    SetSPFFHeaderItem(ifcModel, 0, 1, sdaiUNICODE, WCHAR_STRING);
    SetSPFFHeaderItem(ifcModel, 0, 2, sdaiSTRING, ANSI_SLASH);
    SetSPFFHeaderItem(ifcModel, 0, 3, sdaiUNICODE, WCHAR_SLASH);

    CheckHeader(ifcModel);

    //
    auto wall = IFC4::IfcWall::Create(ifcModel);

    //
    sdaiPutAttrBN(wall, "Name", sdaiSTRING, ANSI_STRING);
    CheckAttr(wall, "Name", ANSI_STRING, WCHAR_STRING, ANSI_STEP);

    //
    sdaiPutAttrBN(wall, "Description", sdaiUNICODE, WCHAR_STRING);
    CheckAttr(wall, "Description", ANSI_STRING, WCHAR_STRING, UNICODE_STEP);

    sdaiPutAttrBN(wall, "ObjectType", sdaiSTRING, ANSI_SLASH);
    CheckAttr(wall, "ObjectType", ANSI_SLASH, WCHAR_SLASH, STEP_SLASH);

    sdaiPutAttrBN(wall, "GlobalId", sdaiUNICODE, WCHAR_SLASH);
    CheckAttr(wall, "GlobalId", ANSI_SLASH, WCHAR_SLASH, STEP_SLASH);

    auto stepId = internalGetP21Line(wall);

    //
    sdaiSaveModelBN(ifcModel, "sdaiSaveModelBN_" REG_CHARS_FILE_NAME);
    CheckRegionalChars("sdaiSaveModelBN_" REG_CHARS_FILE_NAME, stepId);

    //
    fopen_s(&myFileWrite, "engiSaveModelByStream_" REG_CHARS_FILE_NAME, "wb");
    engiSaveModelByStream(ifcModel, WriteCallBackFunction, BLOCK_LENGTH_WRITE);
    fclose(myFileWrite);

    CheckRegionalChars("engiSaveModelByStream_" REG_CHARS_FILE_NAME, stepId);

    //
    unsigned char content[10240];
    int_t size = _countof(content);
    engiSaveModelByArray(ifcModel, content, &size);

    fopen_s(&myFileWrite, "engiSaveModelByArray_" REG_CHARS_FILE_NAME, "wb");
    fwrite(content, 1, size, myFileWrite);
    fclose(myFileWrite);

    CheckRegionalChars("engiSaveModelByArray_" REG_CHARS_FILE_NAME, stepId);

    //
    sdaiSaveModelAsXmlBN(ifcModel, "sdaiSaveModelAsXmlBN.xml");
    //TODO - XML issues
    // CheckRegionalChars("sdaiSaveModelAsXmlBN.xml", stepId);

    sdaiSaveModelAsSimpleXmlBN(ifcModel, "sdaiSaveModelAsSimpleXmlBN.xml");
    //TODO - XML issues
    //CheckRegionalChars("sdaiSaveModelAsSimpleXmlBN.xml", stepId);

    sdaiCloseModel(ifcModel);

}

extern void Encodings(void)
{
    ENTER_TEST;

    EncodingAndFilter();
    PutGetRegionalChars();

}