//*****************************************************************************
// bob: base BobObject
//
// Eli Bendersky (eliben@gmail.com)
// This code is in the public domain
//*****************************************************************************
#ifndef BOBOBJECT_H
#define BOBOBJECT_H

#include <string>
#include <list>


// Abstract base class for all objects managed by the Bob VM. 
// 
// Objects deriving from BobObject are automatically garbage-collected. 
// Therefore, you should only allocate them dynamically with new, and
// never, *ever* explicitly delete them.
//
class BobObject 
{
public:
    BobObject();
    virtual ~BobObject() = 0;

    virtual std::string repr() const
    {
        return "<object>";
    }

    // Derived classes that want to be comparable should override this
    // method. 
    // It shall be invoked only with 'other' of the same class as the
    // object it's invoked on.
    //
    virtual bool equals_to(const BobObject& other) const
    {
        (void)other;
        return false;
    }

    void* operator new(size_t sz);
    void operator delete(void* p);

    // Garbage collection code.
    //
    virtual void gc_mark()
    {
        m_gc_marked = true;
        gc_mark_pointed();
    }

    virtual void gc_clear() 
    {
        m_gc_marked = false;
    }

    virtual bool is_gc_marked() const
    {
        return m_gc_marked;
    }

    // The default implementation does nothing here, to simplify the trivial
    // objects that hold no pointers to other objects
    //
    virtual void gc_mark_pointed()
    {
    }

protected:
    bool m_gc_marked;
};


class BobAllocator 
{
public:
    static BobAllocator& get()
    {
        return the_allocator;
    }

    void* allocate_object(std::size_t sz);
    void release_object(void* p);

    // Run the garbage collector
    //
    void run_gc();

    // Return various statistics as a string for debugging
    //
    std::string stats_general() const;
    std::string stats_all_live() const;

private:
    static BobAllocator the_allocator;

    BobAllocator()
        : total_alloc_size(0)
    {
    }

    ~BobAllocator()
    {
    }

    BobAllocator(const BobAllocator&);
    BobAllocator& operator=(const BobAllocator&);

    std::list<BobObject*> live_objects;
    std::size_t total_alloc_size;
};


// Compare two objects of any type derived from BobObject
//
bool objects_equal(const BobObject*, const BobObject*);


#endif /* BOBOBJECT_H */
