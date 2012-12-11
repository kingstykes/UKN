/*
 *  Exception.h
 *  Hoshizora
 *
 *  Created by Robert Bu on 8/27/10.
 *  Copyright 2010 Robert Bu. All rights reserved.
 *
 */

#ifndef ukn_exception_h
#define ukn_exception_h

#include "mist/Platform.h"
#include "mist/StackTrace.h"
#include <string>
#include <sstream>
#include <typeinfo>

#define MIST_THROW_EXCEPTION(mess) throw mist::Exception(mess,   \
                                                        __FUNCTION__,     \
                                                        __FILE__,         \
                                                        __LINE__)

namespace mist {

	class Exception: public std::exception {
	public:
		Exception(const std::string& mssg, const char* function, const char* file, int line):
        mMssg(mssg),
        mFunction(function),
        mFile(file),
        mLine(line) {

        }

        Exception(const std::string& mssg):
        mMssg(mssg),
        mFunction(0),
        mFile(0),
        mLine(-1) {

        }

        virtual ~Exception() throw() {}

		// get a formatted exception mssg
		virtual const char* what() const throw() {
            std::ostringstream mssg;
            mssg << "mist::Exception: class=" << name() << "\nMessage=" << mMssg;
            if(mFunction != 0) mssg << "\nFunction=" << mFunction;
            if(mFile != 0) mssg << "\nFile=" << mFile;
            if(mLine != -1) mssg << "\nLine=" << mLine;

            return mssg.str().c_str();
        }

        // the name of the exception
        virtual const char* name() const throw() {
            return "mist::Exception";
        }

        // the class name of the exception
        virtual const char* className() const throw() {
            return typeid(*this).name();
        }

		const std::string& mssg() const {
            return mMssg;
        }
		const char* function() const {
            return mFunction;
        }
		const char* file() const {
            return mFile;
        }

		int32 line() const {
            return mLine;
        }

        virtual void rethrow() {
            throw *this;
        }

        std::string getFormattedCallStack() const {
            return ukn_get_formatted_stack_trace_string();
        }

	private:
        std::string mMssg;
        const char* mFunction;
        const char* mFile;
        int32 mLine;
	};


} // namespace sora

#endif
