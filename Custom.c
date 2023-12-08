/*
    File: Custom.h
    Author: hackerman008
	Purpose: Custom implementation of the GetProcAddress() function to retrieve the API address dynamically at runtime
	Modified: 7 December, 2023
	License: MIT License
*/

#include <intrin.h>
#include <ctype.h>
#include <wchar.h>

/*
	For enabling debug messages define /DDEBUG preprocessor symbol for compilation
*/
#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif


/*
    The function accepts 3 parameters.
    param1:
        type: void* 
        description: base address of the dll in which to search for the API 
    param2:
        type: char*
        description: pointer API name string
	param3:
		type: size_t
		description: length of the API name string
*/
void* GetProcAddress__(void* dllBase, char* apiName, size_t apiNameLength){

	DEBUG_PRINT(("\nGetProcAddress__(%p, %s, %llu) ", dllBase, apiName, apiNameLength));
	DEBUG_PRINT(("\n|"));
	DEBUG_PRINT(("\n|"));
	
	//convert name to lowercase
	int i;
	for(i=0; i<apiNameLength; i++){
        apiName[i] = towlower(apiName[i]);                                                       
    }

	//retrieve address of IMAGE_DATA_DIRECTORY
	unsigned long e_lfanew, offsetImageExportDirectory; 
	e_lfanew = *((unsigned long*)((unsigned long long)dllBase + 0x3c));
	offsetImageExportDirectory = *((unsigned long*)((unsigned long long)dllBase + e_lfanew + 0x18 + 0x70));
	void* baseImageExportDirectory = (void*)((unsigned long long)dllBase + offsetImageExportDirectory);
	
	DEBUG_PRINT(("\n[-] e_lfanew = %4x", e_lfanew));
	DEBUG_PRINT(("\n[-] offsetImageExportDirectory = %4x", offsetImageExportDirectory));
	DEBUG_PRINT(("\n[-] baseImageExportDirectory = %p", baseImageExportDirectory));
	
	
	//retrieve number of api names
	unsigned long valNumberOfNames;
	valNumberOfNames = *((unsigned long*)((unsigned long long)baseImageExportDirectory + 0x18));
	DEBUG_PRINT(("\n[-] valNumberOfNames = %lu", valNumberOfNames));
	
	//retrive addresses of required fields: AddressOfNames, AddressOfFunctions, AddressOfNameOrdinals
	void* baseAddressOfFunctions = (void*)(*((unsigned long*)((unsigned long long)baseImageExportDirectory + 0x1c)) + (unsigned long long)dllBase);
	void* baseAddressOfNames = (void*)(*((unsigned long*)((unsigned long long)baseImageExportDirectory + 0x20)) + (unsigned long long)dllBase);
	void* baseAddressOfNameOrdinals = (void*)(*((unsigned long*)((unsigned long long)baseImageExportDirectory + 0x24)) + (unsigned long long)dllBase);
	
	DEBUG_PRINT(("\n[-] baseAddressOfFunctions = %p", baseAddressOfNameOrdinals));
	DEBUG_PRINT(("\n[-] baseAddressOfNames = %p", baseAddressOfNames));
	DEBUG_PRINT(("\n[-] baseAddressOfNameOrdinals = %p", baseAddressOfNameOrdinals));
	
	//retrieve api base and return
	unsigned long nameIterator = 0;
	unsigned long offsetApiName, offsetApiBase;
	void* apiBase;
	size_t lenRetrievedName;
	char* apiNameRetrieved;
	char nameBuffer[100];
	unsigned short valOrdinal;
	do{
		//retrieve api name for comparison
		offsetApiName = *((unsigned long*)((unsigned long long)baseAddressOfNames + nameIterator*4));					//i*4 done to increment according to size of each offset value stored in the array
		apiNameRetrieved = (char*)(offsetApiName + (unsigned long long)dllBase);
		lenRetrievedName = strlen(apiNameRetrieved);
		if(lenRetrievedName != apiNameLength){
			goto NEXT_ITERATION;
		}
		
		DEBUG_PRINT(("\n[-] apiNameRetrieved = %s", apiNameRetrieved));
		DEBUG_PRINT(("\n[-] apiNameRetrieved length = %d", strlen(apiNameRetrieved)));
		strncpy(nameBuffer, apiNameRetrieved, lenRetrievedName);
		
		//convert retrived name to lowercase
		for(i=0; i<lenRetrievedName; i++){
			nameBuffer[i] = tolower(nameBuffer[i]);                                                       
		}
		DEBUG_PRINT(("\n[-] nameBuffer = %s", nameBuffer));
		
		//compare names
		if(strncmp(nameBuffer, apiName, lenRetrievedName) == 0){
			valOrdinal = *((unsigned short*)((unsigned long long)baseAddressOfNameOrdinals + nameIterator*2));			//ordinal value from AddressOfNameOrdinals array retieved
			offsetApiBase = *((unsigned long*)((unsigned long long)baseAddressOfFunctions + valOrdinal*4));				//api offset retrieved from AddressOfFunctions array
			apiBase = (void*)(offsetApiBase + (unsigned long long)dllBase);												//API address
			
			DEBUG_PRINT(("\n[-] valOrdinal = %hu", valOrdinal));
			DEBUG_PRINT(("\n[-] offsetApiBase = %4x", offsetApiBase));
			DEBUG_PRINT(("\n[-] apiBase = %p", apiBase));
			DEBUG_PRINT(("\n\n"));
			return apiBase;
		}
		memset(nameBuffer, 0, sizeof(nameBuffer));
		
		NEXT_ITERATION:
		nameIterator += 1;
	}while(nameIterator < valNumberOfNames);	
	return -1;	
}

/*
    The function accepts 2 parameters.
    param1:
        type: wchar_t* 
        description: name of the DLL whose base is to be retrieved
    param2:
        type: size_t
        description: length of the DLL name string
*/
void* GetDllBase__(wchar_t* dllName, size_t nameLength){

	DEBUG_PRINT(("\nGetDllBase__(%ls, %llu) ", dllName, nameLength));
	DEBUG_PRINT(("\n|"));
	DEBUG_PRINT(("\n|"));
	
    size_t i;
    char dllNameInMem[30];
    char* baseString;
    void* dllBase;
    unsigned long long teb, peb, pebLdrData, inLoadOrderModuleList;
    size_t lenRetrievedDllName;
	teb = (__readgsqword(0x30));                                                                //retrieve TEB base
    DEBUG_PRINT(("\n[-] teb = %p", teb));
    peb = *(unsigned long long*)(teb + 0x60);                                                   //retrieve pointer PEB
	DEBUG_PRINT(("\n[-] peb = %p", peb));
	pebLdrData = *((unsigned long long*)(peb + 0x18));                                          //retrieve pointer to PEB_LDR_DATA
    DEBUG_PRINT(("\n[-] pebLdrData = %p", pebLdrData));
	inLoadOrderModuleList = *((unsigned long long*)(pebLdrData + 0x10));                        //retrieve pointer to InLoadOrderModuleList 
	DEBUG_PRINT(("\n[-] inLoadOrderModuleList = %p", inLoadOrderModuleList));
	
	//convert passed DLL name to lower case
	for(i=0; i<nameLength*2; i+=2){
        dllName[i] = towlower(dllName[i]);                                                       
    }

    //loop till name is found
    CHECK_NEXT_STRING:
    baseString = (char*)*((unsigned long long*)(inLoadOrderModuleList + 0x60));
	lenRetrievedDllName = wcslen(baseString);
	if(lenRetrievedDllName != nameLength){
		inLoadOrderModuleList = *((unsigned long long*)(inLoadOrderModuleList));                //retrieve pointer to next structure in linked list
        DEBUG_PRINT(("\n[-] next pointer LDR_DATA_TABLE_ENTRY = %p", inLoadOrderModuleList));
        //memset(dllNameInMem, 0, sizeof(dllNameInMem));
        goto CHECK_NEXT_STRING;
	}
	
	wcsncpy(dllNameInMem, baseString, lenRetrievedDllName);
	for(i=0; i<lenRetrievedDllName*2; i+=2){
        dllNameInMem[i] = towlower(dllNameInMem[i]);                                           //convert DLL name in structure to lowercase
    }
	DEBUG_PRINT(("\n[-] dllName in LDR_DATA_TABLE_ENTRY = %ls", dllNameInMem));
	
    if(wcsncmp(dllName, dllNameInMem, lenRetrievedDllName) == 0){
        dllBase = *((unsigned long long*)(inLoadOrderModuleList + 0x30));                 		//retrieve DLL base
        DEBUG_PRINT(("\n[-] dllBase = %p\n", dllBase));
        return dllBase;
    }
    else{
        
        inLoadOrderModuleList = *((unsigned long long*)(inLoadOrderModuleList));                //retrieve pointer to next structure in linked list
        DEBUG_PRINT(("\n[-] next pointer LDR_DATA_TABLE_ENTRY = %p", inLoadOrderModuleList));
        memset(dllNameInMem, 0, sizeof(dllNameInMem));
        goto CHECK_NEXT_STRING;                                                                 //continue with next dll name
    }
    return -1;
}
