
#ifndef EASY_UNSPECIFIED_BOOL_H_INCLUDED
#define EASY_UNSPECIFIED_BOOL_H_INCLUDED

namespace easy
{
	namespace detail
	{
		struct _unspecified { void func() {} };
	}

	typedef void (detail::_unspecified::*unspecified_bool_type)();

	inline unspecified_bool_type unspecified_true() {
		return &detail::_unspecified::func;
	}

}


#endif