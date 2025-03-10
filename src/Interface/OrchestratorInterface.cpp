#include "OrchestratorInterface.h"
#include <json-c/json.h>

namespace arrowhead{

OrchestratorInterface::OrchestratorInterface() {}

OrchestratorInterface::~OrchestratorInterface() {}


void OrchestratorInterface::sendRequestToProvider(std::string data, 
				std::string provider_uri, std::string method) {
	sendRequest(data, provider_uri, method);
}

int OrchestratorInterface::sendOrchestrationRequest(
				std::string requestForm, 
				ArrowheadDataExt *config) {

	if(config->SECURE_ARROWHEAD_INTERFACE)
          return SendHttpsRequest(requestForm, config->ACCESS_URI, "POST");
	else
          return sendRequest(requestForm, config->ACCESS_URI, "POST");
}


bool OrchestratorInterface::getOrchetrationRequestForm(
				json_object *&request_form, ArrowheadDataExt &config) {

	//TODO change these to a actual json implementation
    // change return path to handle json
    // cascade throe sensorHandler, OrchestraIntreface, http...
	/*	std::string form = 
    	"{ \"requesterSystem\": { "     
        	"\"systemName\": \""+config.THIS_SYSTEM_NAME+"\", "
            "\"address\": \""+config.THIS_ADDRESS+"\", "
            "\"port\": "+std::to_string(config.THIS_PORT)+", "
            "\"authenticationInfo\": \""+config.AUTHENTICATION_INFO+"\" }, "
		"\"requestedService\": { "      
        	"\"serviceDefinition\": \""+config.SERVICE_NAME+"\", "
            "\"interfaces\": [ \""+config.INTERFACE+"\" ], "
            "\"serviceMetadata\": { "       
            	"\"security\": \""+config.SECURITY+"\" } }, "
		"\"orchestrationFlags\": { "    
        	"\"overrideStore\": "+std::to_string(config.OVERRIDE_STORE)+", " 
            "\"matchmaking\": "+std::to_string(config.MATCHMAKING)+", "
            "\"metadataSearch\": "+std::to_string(config.METADATA_SEARCH)+", "
            "\"pingProviders\": "+std::to_string(config.PING_PROVIDERS)+", "
            "\"onlyPreferred\": "+std::to_string(config.ONLY_PREFERRED)+", "
            "\"externalServiceRequest\": "+std::to_string(config.EXTERNAL_SERVICE_REQUEST)+" }, "
        "\"preferredProviders\": [ { "
        	"\"providerSystem\": { "        
        		"\"systemName\": \""+config.TARGET_SYSTEM_NAME+"\", "
            	"\"address\": \""+config.TARGET_ADDRESS+"\", "
            	"\"port\": \""+std::to_string(config.TARGET_PORT)+"\" } } ] }";
 	fprintf(stderr, "\norigin:\n%s\n", form.c_str());
	*/

	//requesteSystem
	json_object *requester_system = json_object_new_object();

	jsonAddString(requester_system, config.THIS_SYSTEM_NAME, "systemName");
	jsonAddString(requester_system, config.THIS_ADDRESS, "address");
	jsonAddInt(requester_system, config.THIS_PORT, "port");

	jsonAddString(requester_system, config.AUTHENTICATION_INFO, "authenticationInfo");
	


	// requestedService
	json_object *requested_service = json_object_new_object();
	
	jsonAddString(requested_service, config.SERVICE_NAME, "serviceDefinition");

	json_object *jarray = json_object_new_array();
	json_object_array_add(jarray,
					json_object_new_string(config.INTERFACE.c_str()));

	json_object_object_add(requested_service, "interfaces", jarray);

	json_object *service_metadata = json_object_new_object();
	jsonAddString(service_metadata, config.SECURITY, "security");
	json_object_object_add(requested_service, "serviceMetadata", service_metadata);
	
	
	// orchestrationFlags
	json_object *orchestration_flags = json_object_new_object();
	
	jsonAddBool(orchestration_flags, config.OVERRIDE_STORE, "overrideStore");
	jsonAddBool(orchestration_flags, config.MATCHMAKING, "matchmaking");
	jsonAddBool(orchestration_flags, config.METADATA_SEARCH, "metadataSearch");
	jsonAddBool(orchestration_flags, config.PING_PROVIDERS, "pingProviders");
	jsonAddBool(orchestration_flags, config.ONLY_PREFERRED, "onlyPreferred");
	jsonAddBool(orchestration_flags, config.EXTERNAL_SERVICE_REQUEST, "externalServiceRequest");


	// preferdProvider
	json_object *preferred_provider = json_object_new_object();
	jarray = json_object_new_array();
	
	json_object *provider_system = json_object_new_object();

	jsonAddString(provider_system, config.TARGET_SYSTEM_NAME, "systemName");
	jsonAddString(provider_system, config.TARGET_ADDRESS, "address");
	jsonAddInt(provider_system, config.TARGET_PORT, "port");


	json_object_object_add(preferred_provider, "providerSystem", provider_system);
	json_object_array_add(jarray, preferred_provider);
	
	json_object *local = json_object_new_object();
	json_object_object_add(local, "requesterSystem", requester_system);
	json_object_object_add(local, "requestedService", requested_service);
	json_object_object_add(local, "orchestrationFlags", orchestration_flags);
	json_object_object_add(local, "preferredProviders", jarray);
		
	request_form = local;
	return true;
}

void OrchestratorInterface::jsonAddString(json_object* obj, std::string str, const char* name) {
	json_object_object_add(obj, name, 
					json_object_new_string(str.c_str()));
}

void OrchestratorInterface::jsonAddInt(json_object* obj, int nr, const char* name) {
	json_object_object_add(obj, name, json_object_new_int(nr));
}

void OrchestratorInterface::jsonAddBool(json_object* obj, bool boolean, const char* name) {
	json_object_object_add(obj, name, json_object_new_boolean(boolean));
}

///////////////
// Callbacks //
///////////////

// Overload Sensor-handler callback functionality here!
size_t OrchestratorInterface::httpResponseCallback(char *ptr, size_t size) {
	return callbackOrchestrationResponse(ptr, size);
}

size_t OrchestratorInterface::callbackGETHttp(char *ptr, size_t size){
	const char *p = ptr;
	return callbackRequest(p, size);
}

size_t OrchestratorInterface::callbackRequest(const char *ptr, size_t size){
	fprintf(stderr, "no over rider callbackRequest");
	return size;
}

size_t OrchestratorInterface::httpsResponseCallback(char *ptr, size_t size)
{
	return callbackOrchestrationResponse(ptr, size);
}


size_t OrchestratorInterface::callbackOrchestrationResponse(char *ptr, 
				size_t size) {

//
//Expected Response -- example
//{
//  "response": [
//    {
//      "service": {
//        "serviceDefinition": "IndoorTemperature_ProviderExample",
//        "interfaces": [
//          "REST-JSON-SENML"
//        ],
//        "serviceMetadata": {
//          "unit": "Celsius"
//        }
//      },
//      "provider": {
//        "systemName": "SecureTemperatureSensor",
//        "address": "10.0.0.103",
//        "port": 8452,
//        "authenticationInfo": "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAyzDRU+P6h8Jwp9eiGYqqlgoAmPLo6M/PTZX+pkKr2MIg7VLdnjUeXzKFljwJKjYGG3nus53F4RFnymT7VoIQT+SmkuLy90Ir6O3XRWiD74XlOIkthT8/fq5FP9sJIusaRc9jkx3Y8jC3yCz1BPJDa+0A+heWarN+K7W7985aBFiJ1ycsB7yJFYAt7wVRc2fkgGpmp4l34Ta4J7QVwzYBOx5w5hIE29EzXOhl0GB6c/licclhisOnN31OWizoWJWAdexmjR9ugHgFSv4eUbjQ3/Qc0tM3ljmbnMMmj54fKZHtpesLXrCi44aQ88e7UOd/xplAbntEPvz168oie4IzFQIDAQAB"
//      },
//      "serviceURI": "/1/1/REST-JSON-SENML",
//      "instruction": null,
//      "authorizationToken": "t3AeN6Q0xa177mV+9WkQlsLL3arBnNrYm/sZ/OJTSSrpGRBz3alaQT+0ZckJrg3brIE2ofzyzYs0wKxwQbW9T12T+hW0DeFyN3hmxTZk0Kaw9h56M9p4N6srlid8Ewu5qesPLQLbQ1AF3Q7HCy7uKIbDjTeGdd1IZmae/QP8CjhnVRu1uZreiFVzCf9HUsQpfnRbssrjtXkmvcWXbxx1BD4mFCEI6Ze5CnTld+GSWUawM39byz5cQbq2cBrecKfGC5zQh1vHFvTxrinq5ARnO3S9Gaul463P0QO3zWEFehbIscvEwAC6rUx33NMjdTqdfXd7n8VdED0yMbWaQ86ApQ==",
//      "signature": "W6ODMMDD5xFlT3UycKBSsbJ5pskQds69nmiZ0cXj+KrblVdkoTsYRgzoFBodw+XLBf8MvDphfMDYqihjzPmq1xMj0v+AkEB6nV3uMTznR9cEW9VOgGBVZtRt+h9Sh5HNe2jRblLD7+kimmclCU5zqqHZEOrqShaZGuNEJeZg41eokuFBzARTWlckyM5kTM+GgalFtpMevGuqBd5FcpYVC1TNoMlABIYUqPzrHsKFsRifLSMeqLnvGmWZB/HKDDl2iQ+jPG58DcxdoqPcLAd+vzq/+oir0n/OHH1DFvdyjQ7NMIc8u1sz+uhm1jOQZlmgl2O5aPWDoD3mSb0Zduv2Nw=="
//    }
//  ]
//}
//
    printf("Orchestration response: %s\n", ptr);

    std::string token;
    std::string signature;
    std::string sIPAddress;
	uint32_t uPort;
	std::string sInterface;
	std::string sURI;

    struct json_object *obj = json_tokener_parse(ptr);
    if(obj == NULL){
         fprintf(stderr, "Error: could not parse orchestration response\n");
         return 1;
    }

    struct json_object *jResponseArray;
    if(!json_object_object_get_ex(obj, "response", &jResponseArray)){
         fprintf(stderr, "Error: could not parse response\n");
         return 1;
    }

    struct json_object *jResponse = json_object_array_get_idx(jResponseArray, 0);
    struct json_object *jProv2;

    if(!json_object_object_get_ex(jResponse, "provider", &jProv2)){
         fprintf(stderr, "Error: could not parse provider section\n");
         return 1;
    }

    struct json_object *jAddr;
    struct json_object *jPort;
    struct json_object *jService;
    struct json_object *jIntf;
    struct json_object *jIntf0;
    struct json_object *jUri;
    struct json_object *jToken;
    struct json_object *jSignature;

	if(!json_object_object_get_ex(jProv2,    "address",    &jAddr)) {
		fprintf(stderr, "Error: could not find address\n");
		return 1;
	}
    if(!json_object_object_get_ex(jProv2,    "port",       &jPort)) {
		fprintf(stderr, "Error: could not find port\n");
		return 1;
	}
    if(!json_object_object_get_ex(jResponse, "service",    &jService)) {
		fprintf(stderr, "Error: could not find service\n");
		return 1;
	}
    if(!json_object_object_get_ex(jService,  "interfaces", &jIntf)) {
		fprintf(stderr, "Error: could not find interface\n");
		return 1;
	}
    if(!json_object_object_get_ex(jResponse,  "serviceURI", &jUri)) {
		fprintf(stderr, "Error: could not find serviceURI\n");
		return 1;
	}

    jIntf0 = json_object_array_get_idx(jIntf, 0);

    if(jIntf0 == NULL){
         fprintf(stderr, "Error: could not find interface\n");
         return 1;
    }

	sIPAddress = std::string(json_object_get_string(jAddr));
	uPort      = json_object_get_int(jPort);
	sInterface = std::string(json_object_get_string(jIntf0));
    sURI       = std::string(json_object_get_string(jUri));

	 /*
	 if(config.SECURE_PROVIDER_INTERFACE){
     	if(!json_object_object_get_ex(jResponse,  "authorizationToken", &jToken)){
			fprintf(stderr, "Error: could not find authorizationToken\n");
			return 1;
		}
		if(!json_object_object_get_ex(jResponse,  "signature",          &jSignature)){
			fprintf(stderr, "Error: could not find signature\n");
			return 1;
		}

          token     = string(json_object_get_string(jToken));
          signature = string(json_object_get_string(jSignature));

          //https://ipaddr:port/serviceURI?token=_token_&signature=_signature_
          target_uri = "https://" + sIPAddress + ":" + std::to_string(uPort) + "/" + sURI + "?token=" + token + "&signature=" + signature;
     }
     else{
	 */
    target_uri= "http://" + sIPAddress + ":" + std::to_string(uPort) + "/" + sURI;
    // }
	return size;
}

////////////////////
// Callbacks ends //
////////////////////

}
