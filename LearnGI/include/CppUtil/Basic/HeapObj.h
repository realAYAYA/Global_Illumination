#ifndef _HEAP_OBJ_H_
#define _HEAP_OBJ_H_

#include <CppUtil/Basic/Ptr.h>

#define HEAP_OBJ_SETUP(CLASS) \
public:\
	typedef CppUtil::Basic::Ptr<CLASS> Ptr;\
	typedef CppUtil::Basic::CPtr<CLASS> CPtr;\
	virtual const char * GetClassName(){ return "#CLASS"; }\
	Ptr This(){ return std::dynamic_pointer_cast<CLASS>(shared_from_this()); }\
	CPtr CThis() const { return std::dynamic_pointer_cast<const CLASS>(shared_from_this()); }\
protected:\
	virtual ~CLASS() = default;

namespace CppUtil {
	namespace Basic {
		template <typename T>
		Ptr<T> ToPtr(T* op) {
			return Ptr<T>(op, T::ProtectedDelete);
		}

		template <typename T>
		CPtr<T> ToCPtr(const T* op) {
			return CPtr<T>(op, T::ProtectedDelete);
		}

		class HeapObj : public std::enable_shared_from_this<HeapObj> {
		public:
			template <typename T>
			friend Ptr<T> ToPtr(T* op);
			template <typename T>
			friend CPtr<T> ToCPtr(const T* op);
		protected:
			virtual ~HeapObj() = default;
			static void ProtectedDelete(const HeapObj* op);
			using std::enable_shared_from_this<HeapObj>::shared_from_this;
		};
	}
}

#endif // !_HEAP_OBJ_H_


/* 此宏的作用是使类变为一个强制转换过的智能指针，用于其他场景，以下语义分别是
* 声明一组模板为自己类型的智能指针
* 获取类的名称
* 一组方法，返回将自己进行强制转换后返回一个指向自己的指针
*/