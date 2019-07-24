#pragma once
#include <iostream>
#include <json-c/json.h>

// these params need to be set
// they have to agree whit arrowhead core to work

namespace arrowhead{
	// struct contains all configure variables
	class ArrowheadDataExt {
	private:
		void set(std::string settings);
		void setCore(json_object *core);
		void setOrchestration(json_object *orchestation);
		void setThis(json_object *this_system);
		void setTarget(json_object *target_system);
		void setService(json_object *service);

		std::string getString(std::string name, json_object *src);
		bool getBool(std::string name, json_object *src);
		int getInt(std::string name, json_object *src);

	public:
		// service settings
		std::string SERVICE_DEFINITION;
		std::string INTERFACE;
		std::string	SERVICE_URI;
		std::string UNIT;
		std::string SECURITY;
	
		// connect to core
		std::string ACCESS_URI;
		std::string ACCESS_URI_HTTPS;
	
		// orchestrator setting
		bool OVERRIDE_STORE;
		bool MATCHMAKING;
		bool METADATA_SEARCH;
		bool PING_PROVIDERS;
		bool ONLY_PREFERRED;
		bool EXTERNAL_SERVICE_REQUEST;

		// this procumer
		std::string	THIS_SYSTEM_NAME;
		std::string THIS_ADDRESS;
		std::string THIS_ADDRESS6;
		int THIS_PORT;

		// target procumer
		std::string TARGET_SYSTEM_NAME;
		std::string TARGET_ADDRESS;
		int TARGET_PORT;
	
		// security 
		bool SECURE_ARROWHEAD_INTERFACE;
		bool SECURE_PROVIDER_INTERFACE;
		std::string PUBLIC_KEY_PATH;
		std::string PRIVATE_KEY_PATH;
		std::string AUTHENTICATION_INFO;
		
		ArrowheadDataExt();
		~ArrowheadDataExt();

		void print();
		void lood(std::string file_path);
	
	};
}
