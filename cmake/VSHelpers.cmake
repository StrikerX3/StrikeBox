# Add Visual Studio filters to better organize the code
function(vs_set_filters SOURCES)
	if(MSVC)
		set(extra_macro_args ${ARGN})
		list(LENGTH extra_macro_args num_extra_args)
	    if(${num_extra_args} GREATER 0)
	        list(GET extra_macro_args 0 optional_arg)
	        set(BASE_DIR "${optional_arg}")
	    else()
	    	set(BASE_DIR "")
	    endif()

		foreach(FILE ${SOURCES}) 
		    # Get the directory of the source file
		    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

		    # Remove common directory prefix to make the group
		    if(BASE_DIR STREQUAL "")
		    	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
		    else()
		    	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/${BASE_DIR}" "" GROUP "${PARENT_DIR}")
		    endif()

		    # Make sure we are using windows slashes
		    string(REPLACE "/" "\\" GROUP "${GROUP}")

		    # Group into "Source Files" and "Header Files"
		    if("${FILE}" MATCHES ".*\\.c(pp)?")
		       set(GROUP "Source Files${GROUP}")
		    elseif("${FILE}" MATCHES ".*\\.h")
		       set(GROUP "Header Files${GROUP}")
		    endif()

		    source_group("${GROUP}" FILES "${FILE}")
		endforeach()
	endif()
endfunction()

# Make the Debug and RelWithDebInfo targets use Program Database for Edit and Continue for easier debugging
function(vs_use_edit_and_continue)
	if(MSVC)
		string(REPLACE "/Zi" "/ZI" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
		string(REPLACE "/Zi" "/ZI" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
		set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG} PARENT_SCOPE)
		set(CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO} PARENT_SCOPE)
	endif()
endfunction()
