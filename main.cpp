#include <stdio.h>

class Victim {
public:
    virtual int fn(void *vp) {
        return printf("%p.fn(%p)\n", this, vp);
    }
};

class VTable {
public:
    const void **vtbl;

    template <typename Member>
    const void *get(const Member fn) const {
        const void *vidx = *(void**)(&fn);  //TODO: pointers to virtual are 64-bit
        size_t idx = (size_t)vidx;
        idx--;
        printf("GET %p.%p[%p] = %p\n", this, vtbl, vidx, vtbl[idx]);
        return vtbl[idx];
    }

    template <typename Member, typename Repr>
    void *set(const Member fn, const Repr value) {
        const void *vrep = *(void**)(&value);
        const void *vidx = *(void**)(&fn);
        size_t idx = (size_t)vidx;
        idx--;
        printf("SET %p.%p[%p] = %p\n", this, vtbl, vidx, vrep);
        vtbl[(size_t)idx] = vrep;
    }
};

int thisfn(void *self, void *vp) {
    // return printf("%p.tn(%p)\n", self, vp);
    return 0;
}

int main() {
    Victim v;
    setbuf(stdout, NULL);

    v.fn((void*)0xBEEF);

    VTable *t = (VTable*)(&v);
    printf("%I64p = %p\n", &v.fn, t->get(&v.fn));

    t->set(&v.fn, &thisfn);
    printf("%I64p = %p\n", &v.fn, t->get(&v.fn));
    //v.fn((void*)0xCAFE);

    return 0;
}