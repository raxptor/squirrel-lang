/*	see copyright notice in squirrel.h */
#ifndef _SQUTILS_H_
#define _SQUTILS_H_

#define sq_new(__ptr,__type) {__ptr=(__type *)sq_vm_malloc(sizeof(__type));new (__ptr) __type;}
#define sq_delete(__ptr,__type) {__ptr->~__type();sq_vm_free(__ptr,sizeof(__type));}
#define SQ_MALLOC(__size) sq_vm_malloc(__size);
#define SQ_FREE(__ptr,__size) sq_vm_free(__ptr,__size);
#define SQ_REALLOC(__ptr,__oldsize,__size) sq_vm_realloc(__ptr,__oldsize,__size);

//sqvector mini vector class, supports objects by value
template<typename T> class sqvector
{
public:
	sqvector()
	{
		_vals = NULL;
		_size = 0;
		_allocated = 0;
	}
	sqvector(const sqvector<T>& v)
	{
		copy(v);
	}
	void copy(const sqvector<T>& v)
	{
		resize(v._size);
		for(unsigned int i = 0; i < v._size; i++) {
			new ((void *)&_vals[i]) T(v._vals[i]);
		}
		_size = v._size;
	}
	~sqvector()
	{
		if(_allocated) {
			for(unsigned int i = 0; i < _size; i++)
				_vals[i].~T();
			SQ_FREE(_vals, (_allocated * sizeof(T)));
		}
	}
	void reserve(unsigned int newsize) { _realloc(newsize); }
	inline void resize(unsigned int newsize, const T& fill = T())
	{
		if(newsize > _allocated)
			_realloc(newsize);
		if(newsize > _size) {
			while(_size < newsize) {
				new ((void *)&_vals[_size]) T(fill);
				_size++;
			}
		}
		else{
			for(unsigned int i = newsize; i < _size; i++) {
				_vals[i].~T();
			}
			_size = newsize;
		}
	}
	inline void shrinktofit() { if(_size > 4) { _realloc(_size); } }
	inline T& top() const { return _vals[_size - 1]; }
	inline unsigned int size() const { return _size; }
	inline bool empty() const { return (_size <= 0); }
	inline void push_back(const T& val = T())
	{
		if(_allocated <= _size)
			_realloc(_size * 2);
		new ((void *)&_vals[_size++]) T(val);
	}
	inline void pop_back()
	{
		_size--; _vals[_size].~T();
	}
	void insert(unsigned int idx, const T& val)
	{
		resize(_size + 1);
		for(unsigned int i = _size - 1; i > idx; i--) {
			_vals[i] = _vals[i - 1];
		}
    	_vals[idx] = val;
	}
	void remove(unsigned int idx)
	{
		_vals[idx].~T();
		if(idx < (_size - 1)) {
			memcpy(&_vals[idx], &_vals[idx+1], sizeof(T) * (_size - idx - 1));
		}
		_size--;
	}
	inline unsigned int capacity() { return _allocated; }
	inline T &back() const { return _vals[_size - 1]; }
	inline T& operator[](unsigned int pos) const{ return _vals[pos]; }
	T* _vals;
private:
	void _realloc(unsigned int newsize)
	{
		newsize = (newsize > 0)?newsize:4;
		_vals = (T*)SQ_REALLOC(_vals, _allocated * sizeof(T), newsize * sizeof(T));
		_allocated = newsize;
	}
	unsigned int _size;
	unsigned int _allocated;
};

#endif //_SQUTILS_H_
