/** @file LSCP_service.h
 *  @brief class definition for the LSCP library module
 *  
 *  When instantiated, this class provides the application with the following capabilities:
 *  1) receive LSCP packets
 *  2) decode incoming LSCP setting/command messages and automatically generate response messages via application provided callbacks
 *  3) generate outgoing LSCP setting/command messages using application provided callbacks
 *  
 *  This header file also contains the struct definitions for LSCP command and setting callback functions
 *  that the application needs to adhere to and implement.
 *  
 *  Finally, the application code is responsible for wiring their instance of LSCP_service up
 *  with a low level communications library that is derived from type Icomms_circular_buffer.
 *  
 *    
 *  @author Adam Porsch
 *  @bug No known bugs.
 */


#ifndef LSCP_SERVICE_H_
#define LSCP_SERVICE_H_

#include "cJSON.h"
#include "Icomms_circular_buffer.h"

//LSCP_Rx_state_type is the return type of member function run_packet_reception_and_message_processing_state_machine()
typedef enum {WAITING_FOR_NEW_PACKET, RECEIVE_STX, RECEIVE_LENGTH, RECEIVE_JSON_OBJECT, RECEIVE_ETX, PROCESS_PACKET, PACKET_HAS_BEEN_PROCESSED} LSCP_Rx_state_type;
typedef enum {NO_RESPONSE_REQUIRED, RESPONSE_REQUIRED} LSCP_response_required_type;

typedef struct
{
	const char *name;		//the character string representing the name of the LSCP setting message
	
	/**
	 * @brief callback function to process the contents of an incoming local setting message 
	 * 
	 * This callback is invoked when the LSCP library needs to process an incoming local setting message. 
	 * The LSCP message data field is passed to this callback which is implemented in the application code.
	 * That implementation will extract the expected values from the cJSON object, validate and apply the 
	 * local settings as needed.
	 * 
	 * A typical use case of this callback is the scenario where the application functions as a server, and a client
	 * has sent a setting message to it, mandating that the server change one of its local settings.
	 * 
	 * If the application is functioning as a client, then it is less likely that the application will need to handle
	 * an incoming setting message. In this case, a NULL can be used for the callback definition.
	 * 
	 * @param LSCP_data_object_to_be_applied the data field of the LSCP message, encoded as a cJSON object type
	 * 
	 * @return void 
	 */
	void   (*local_setting_msg_cb)(cJSON *LSCP_data_object_to_be_applied);
	
	/**
	 * @brief callback function to process the contents of an incoming setting response message
	 * 
	 *  When an incoming LSCP local setting response message arrives, this callback is invoked. The LSCP message 
	 *  data field is passed to this callback which is implemented in the application code.
	 *  That implementation will extract the expected values from the cJSON object and make a decision on what
	 *  to do with the information contained in the response.
	 *  
	 *  A typical use case of this callback is the scenario where the application functions as a client, and a server 
	 *  has sent a message back to the client in response to its original setting message. The client may choose to take 
	 *  some action, if, for example, the server had to limit/reject the setting and provided feedback of that via
	 *  this setting response message.
	 *  
	 *  If the application is functioning as a server, then this callback is unlikely to be used, as it is unlikely
	 *  that a server will generate a setting message and require a client to issue a response back to them. In this case, 
	 *  a NULL can be used in the callback definition. 
	 *
	 * 
	 * @param LSCP_data_object_to_be_applied
	 * 
	 * @return void
	 */
	void   (*remote_setting_response_msg_cb)(cJSON *LSCP_data_object_to_be_applied);	
	
	/**
	 * @brief callback function to generate the LSCP message data field for either an outgoing setting or setting response message
	 * 
	 * Typically, whether the application is acting as a client or server, settings reside in application memory. This callback
	 * function is responsible for retrieving the settings from application memory, generating a cJSON object based on
	 * the specific setting data field definition (as defined by the application specific LSCP message definition), and 
	 * return that cJSON object that it may be formed into a complete LSCP setting message and sent out.
	 * 
	 * This callback is invoked by the LSCP library when either a local setting response or a remote setting message 
	 * needs to be generated and transmitted.
	 * 
	 * @param none
	 * 
	 * @return cJSON*
	 */
	cJSON* (*generate_data_field_for_either_setting_or_setting_response_msg_cb)(void);
	
} setting_message_callback_keys_type;

typedef struct
{
	const char *name;		//the character string representing the name of the LSCP command message

	/**
	 **The LSCP message data field is passed to this callback which is implemented in the application code.
	 * That implementation will extract the expected values from the cJSON data struct.
	 
	 * @brief callback function to process the data field of an incoming LSCP local command message and generate a data field for the response
	 * 
	 * When an incoming LSCP local command message arrives, this callback is invoked by the LSCP library. The LSCP
	 * message data field is passed to this callback which is implemented in the application code. That implementation 
	 * will extract the expected values from the cJSON object, validate them, and execute the command.
	 * 
	 * If the command, by definition, is requesting data, this callback will also form the data field needed
	 * for the subsequent LSCP local command response message.
	 * 
	 * There are scenarios where incoming local command messages may not have any data parameters. Also, some
	 * local command messages may not require any data in the subsequent command response message. In those cases, 
	 * a NULL may be passed in and/or returned from this callback.
	 * 
	 * The typical use case for this callback is when the application is acting as a server and the client has
	 * issued a command to the server that needs to be executed and may require a response. 
	 * 
	 * If the application if functioning as a client, it is unlikely that it will be receiving any local commands 
	 * that the server needs it to execute. In those cases, this callback can be defined as a NULL.
	 * 
	 * @param *LSCP_data_object_to_be_applied pointer to the incoming command message data field encoded as a cJSON object
	 * 
	 * @return cJSON* the LSCP local command response data field that is a result of the command being executed
	 */
	cJSON* (*local_command_and_associated_response_msg_cb)(cJSON *LSCP_data_object_to_be_applied);
	
	/**
	 * @brief callback function to generate the data field for an outgoing LSCP remote command message
	 * 
	 * This callback is invoked by the LSCP library when the application needs to generate a LSCP remote command message.
	 * Typically this would be done when the application is functioning as a client and needs the server
	 * to execute a specific command.
	 * 
	 * @param cmd_param command specific data parameters that will be used to generate the data field in the outgoing remote command message
	 * 
	 * cmd_param is a void pointer since the application may have a variety of data types that could be used to generate 
	 * the command parameters specified in the data field of an outgoing command message. It is the responsibility of 
	 * specific command callback to cast cmd_param to the correct data type and encode it properly as a cJSON data type. 
	 * This field can be NULL if the outgoing remote command message does not require a data parameter
	 * 
	 * @return cJSON* the LSCP remote command message data field needed to send the outgoing message
	 */
	cJSON* (*remote_command_msg_cb)(void *cmd_param);
	
	/**
	 * @brief callback function to process the data field of an incoming LSCP remote command response message
	 * 
	 * This callback is invoked when the LSCP library needs to process an incoming LSCP remote command response message.
	 * Typically this occurs when the application is functioning as a client and needs to process the 
	 * command response that the server returned in response to the remote command the client originally generated.
	 * 
	 * @param *LSCP_incoming_data_object_to_be_processed the LSCP data field of the incoming command response message
	 * 
	 * @return none
	 */
	void   (*remote_command_response_msg_cb)(cJSON *LSCP_incoming_data_object_to_be_processed);

} command_message_callback_keys_type;


class LSCP_service
{
	public:			
		/**
		 * @brief initialization routine for an instantiated object of type LSCP_service
		 * 
		 * Because the design intent was to statically instantiate instances of this class, a traditional
		 * constructor was not developed for this class. Therefore, this init function must be called before
		 * the LSCP_service object can be used.
		 * 
		 * This LSCP_service gets wired up to the lower level communications library by accepting a pointer to
		 * an instance of a class derived from the Icomms_circular_buffer abstract class (interface).
		 * 
		 * @param comms_circular_bufffer pointer to object of a class derived from Icomms_circular_buffer.
		 * @param rx_packet_buffer pointer to the buffer that contains incoming serialized LSCP messages 
		 * @param tx_packet_buffer pointer to the buffer that contains outgoing serialized LSCP packets 
		 * @param setting_message_callbacks pointer to array of function callbacks defined by the application to handle setting messages
		 * @param number_of_setting_callback_keys the number of unique settings defined in the setting callback array
		 * @param command_message_callbacks pointer to array of function callbacks defined by the application to handle command messages
		 * @param number_of_command_callback_keys the number of unique settings defined in the command callback array
		 * 
		 * @return void
		 */
		void init(Icomms_circular_buffer *comms_circular_bufffer,
					   char *rx_packet_buffer,
					   char *tx_packet_buffer,
					   const setting_message_callback_keys_type *setting_message_callbacks,
					   uint32_t number_of_setting_callback_keys,
					   const command_message_callback_keys_type *command_message_callbacks,
					   uint32_t number_of_command_callback_keys);
					   		
		/**
		 * @brief state machine that receives incoming LSCP packets and processes them
		 * 
		 * This LSCP packet reception state machine needs to be called periodically by the application, typically from the 
		 * main while(1) loop. Each time it is called, it checks for incoming LSCP packets using the Icomms_circular_buffer object
		 * provided to init function. If a valid LSCP message is received, its message is decoded, executed/applied via the 
		 * application provided callbacks, and a response is issued if specified by the command/setting message in question.
		 * 
		 * @param none
		 * 
		 * @return LSCP_Rx_state_type returns the current state of execution
		 * 
		 * The application will not typically need to know the state each time the function is called. The state is returned
		 * because it is required for the wait_for_message_response() function to operate properly.
		 */
		LSCP_Rx_state_type run_packet_reception_and_message_processing_state_machine(void);
		
		
		/**
		 * @brief generates an LSCP setting message based on the setting name string provided to the function and transmits it.
		 * 
		 * The application calls this function when it needs to generate an outgoing setting message.
		 * 
		 * Note: The remote/local label isn't used here since this function can be called by the application when:
		 *	1) An applications "local" setting needs to be sent to a "remote" receiver (i.e. reading update packet being sent to android board)
		 *  2) The application is acting as client and needs to send a "remote" setting to the server that will process it (i.e. the "main" 
		 *     embedded ARM sending settings down to the "input" embedded ARM).
		 * 
		 * @param name the string literal of the LSCP setting message to be generated
		 * @param LSCP_response_required_type tells the LSCP library whether a response is required for this outgoing setting message
		 * 
		 * Per the LSCP protocol definition, if the ID field is present in a message, the receiver is expected to issue a response.
		 * Under the hood, this will cause the ID field to be generated in the outgoing LSCP setting message.
		 * 
		 * @return uint32_t this is the ID field of the outgoing setting message.
		 * 
		 * The application needs to pass this returned ID field to wait_for_message_response() so it knows when to stop blocking and continue execution
		 * once the message response has been received.
		 * 
		 */
		uint32_t generate_and_transmit_setting_message(const char *name, LSCP_response_required_type response_required);		
			
		/**
		 * @brief generates an LSCP remote command message based on the command name string provided to the function and transmits it
		 * 
		 * A typical use case is an application acting as a client that needs to issue a remote command to a server and expects a subsequent response.
		 * In this scenario, the client application will need to block as it waits for the response.
		 * 		 
		 * @param name	string literal of the LSCP setting message to be generated
		 * @param response_required tells the LSCP library whether a response is required for this outgoing remote command message
		 * 
		 * If a response is required, then the applications call to this generate_remote_command_message() function call is used 
		 * in conjunction with a call to wait_for_message_response()
		 * 
		 * @param remote_command_data_param command specific data parameters that will be used to generate the data field in the outgoing remote command message
		 * 
		 * remote_command_data_param is a void pointer since the application may have a variety of data types that could be used to generate
		 * the command parameters specified in the data field of an outgoing remote command message. 
		 * This field can be NULL if the outgoing remote command message does not require a data parameter
		 * 
		 * @return uint32_t this is the ID field of the outgoing remote command message.
		 * 
		 * The application needs to pass this returned ID field to wait_for_message_response() so it knows when to stop blocking and continue execution
		 * once the message response has been received.
		 */
		uint32_t generate_and_transmit_remote_command_message(const char *name, LSCP_response_required_type response_required, void *remote_command_data_param);
		
		/**
		 * @brief waits for an incoming remote command message response that matches the ID provided
		 * 
		 * If the application has called either generate_remote_command_message() or generate_setting_message() and is expecting a response
		 * before it can move on, this function needs to be called in a blocking loop until it returns a 0 indicating the desired
		 * response has been received. The application is responsible for setting up the timeout condition of when to stop waiting
		 * and handle that error condition.
		 * 
		 * Because the application code is blocking in this scenario, wait_for_message_response() takes over and makes the calls
		 * to run_packet_reception_and_message_processing_state_machine().
		 * 
		 * @param expected_ID the message ID of the outgoing remote command message of who's response we're waiting on
		 * 
		 * @return bool returns a 1 if we're still waiting for a response and 0 if the response has been received AND the ID values match
		 */
		bool wait_for_message_response(uint32_t expected_ID);	
	
	private:
		
		typedef enum {SETTING = 0, SETTING_RESPONSE, COMMAND, COMMAND_RESPONSE, EXCEPTION_RESPONSE} LSCP_message_type_field_type;
		typedef enum {ID_PRESENT = 0, ID_NOT_PRESENT} LSCP_id_field_present_type;
		
		void generate_and_transmit_local_setting_response_message(const char *name, uint32_t response_id_field);
		void generate_and_transmit_local_command_response_message(const char *name, uint32_t response_id_field, cJSON *outgoing_message_data_field);		
		
		/**
		 * @brief used by LSCP library to generate and transmit exception messages
		 * 
		 * An exception response message is sent by the receiver of either a setting or command message if it was unable to process
		 * the message and the sender expects a response (id field is set).
		 * 
		 * @param name the string of the message name causing the exception
		 * @param error_message the string to be displayed in the data field of the error message
		 * @param LSCP_ID_field the ID field of the message causing the exception
		 * 
		 * @return void
		 */
		void generate_and_transmit_exception_message(const char *name, const char *error_message, uint32_t LSCP_ID_field);
        
		/**
		 * @brief retrieves the latest LSCP message ID value 
		 * 
		 * When the initiator of a message, whether it be the client or server, expects a response to their outgoing message,
		 * the LSCP protocol accommodates for this by specifying that when the ID field of a message is present, the receiver
		 * is required to provide a response.
		 * 
		 * The ID is an arbitrary, sequential identification number generated by the sender.
		 * 
		 * @param 
		 * 
		 * @return uint32_t
		 */
		uint32_t get_latest_outgoing_message_id(void);
		
		cJSON* generate_data_field_for_either_setting_or_setting_response_message(const char *name);
		
		/**
		 * @brief Builds up a LSCP message packet and initiates the transmission of that packet
		 * 
		 * An LSCP message consists of the following 4 fields:
		 * 1)Type - Integer enumeration describing the type of message
		 * 2)Name - A string identifying the name of the command or setting
		 * 3)Data - JSON encoded object or array 
		 * 4)ID - Unique, sequential integer uniquely identifying a specific LSCP message
		 * 
		 * @param name The name of the command or setting message that needs to be formulated
		 * @param message_id_field The ID value intended for message packet being formulated
		 * @param id_field_is_present Tells the function if the outgoing packet being formed needs to have the ID field or not.
		 * @param message_type_field Tells the function how to populate the LSCP message "type" field
		 * @param outgoing_message_data_field the JSON encoded data object or array for the outgoing LSCP message
		 * 
		 * @return void
		 */
		void form_message_and_initiate_transmission(const char *name, 
													uint32_t message_id_field, 
													LSCP_id_field_present_type id_field_is_present, 
													LSCP_message_type_field_type message_type_field, 
													cJSON *outgoing_message_data_field);		
		
		/**
		 * @brief Forms a LSCP packet and initiates transmission of the packet
		 * 
		 * An LSCP packet consists of:
		 * 1) Start of text (0x02, 1 byte)
		 * 2) Length Word (2 bytes)
		 * 3) JSON encoded serialized LSCP message (n bytes)
		 * 4) End of text (0x03, 1 byte)
		 * 
		 * @param serialized_LSCP_message_string 
		 * 
		 * @return void
		 */
		void form_packet_and_initiate_transmisssion(char *serialized_LSCP_message_string);
		
		/**
		 * @brief deserializes an LSCP message string encoded in JSON, processes it, and if necessary, formulates response packet and initiates transmission
		 * 
		 * It should be noted, this function, via the cJSON libraries, dynamically allocates memory using malloc() in order to deserialize the 
		 * LSCP message using cJSON linked list structs
		 * 
		 * @param message_to_be_processed LSCP serial message encoded in JSON
		 * 
		 * @return uint32_t LSCP message ID field of the incoming packet
		 */
		uint32_t deserialize_and_process_message(char* serialized_message_to_be_processed);

        /**
         * @brief determines if the incoming setting or command has a missing handler & issues exception message
         *          
         * Called by deserialize_and_process_message(), this function is called once the respective callback keys have been
         * traversed through. This function will check the index value against the defined number of keys and issue
         * a exception message if we couldn't find a key match AND if the ID field was present on the original message.
         * 
         * @param index the index used to traverse through the setting/message callback keys
         * @param number_of_callback_keys the the number of callback keys for either settings or commands
         * @param id_field_is_present_flag used to determine if we issue an exception. If id field isn't present, no exception is issued.
         * @param json_incoming_root_object needed by the function to retrieve the setting/command name used for key comparisons
         * @param id_field in case an exception is issued, this ID field from the original incoming message is needed for the response
         * @param error_message the string literal used in the exception message
         * 
         * @return void
         */
        void check_for_missing_handler_and_issue_exception_message(uint32_t index, 
                                                                   uint32_t number_of_callback_keys,
                                                                   bool id_field_is_present_flag,
                                                                   cJSON * json_incoming_root_object,
                                                                   uint32_t id_field,
                                                                   const char *error_message);
		
		Icomms_circular_buffer *comms_circular_buffer = NULL;
		char *rx_message_buffer;				//STX, length, and ETX bytes aren't stored during reception, just the serialized message, hence the "message" label
		char *tx_packet_buffer;					//tx buffer needs to include STX, length, and ETX bytes, hence the "packet" label.
		
		const command_message_callback_keys_type *command_message_callback_keys;
		const setting_message_callback_keys_type *setting_message_callback_keys;
		
		uint32_t number_of_setting_callback_keys;
		uint32_t number_of_command_callback_keys;
		
		bool awating_message_response_flag;
		uint32_t latest_outgoing_message_id;
		uint32_t latest_returned_message_ID;
};



#endif /* LSCP_SERVICE_H_ */