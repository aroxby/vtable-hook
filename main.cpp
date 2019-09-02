#include <cstdio>
#include <cstring>

class Victim {
public:
    virtual int fn(void *vp) {
        return printf("%p.fn(%p)\n", this, vp);
    }
};

class Dummy {
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
        // printf("GET %p.%p[%p] = %p\n", this, vtbl, vidx, vtbl[idx]);
        return vtbl[idx];
    }

    template <typename Member, typename Repr>
    void *set(const Member fn, const Repr value) {
        const void *vrep = *(void**)(&value);
        const void *vidx = *(void**)(&fn);
        size_t idx = (size_t)vidx;
        idx--;
        // printf("SET %p.%p[%p] = %p\n", this, vtbl, vidx, vrep);
        vtbl[(size_t)idx] = vrep;
    }
};

void install_new_vtable(void *obj, int num_members = 1) {
    const static size_t sz = num_members * sizeof(void*);
    VTable *old_table = (VTable*)obj;
    VTable new_table;
    new_table.vtbl = new const void*[num_members / sizeof(void*)];
    memcpy(new_table.vtbl, old_table->vtbl, sz);
    memcpy(obj, &new_table.vtbl, sizeof(new_table.vtbl));
}

int __thiscall thisfn(void *self, void *vp) {
    return printf("%p.tn(%p)\n", self, vp);
}

int main() {
    setbuf(stdout, NULL);
    Victim *v = new Victim;
    VTable *t;

    v->fn((void*)0xBEEF);

    t = (VTable*)(v);
    printf("%I64p = %p\n", &v->fn, t->get(&v->fn));

    install_new_vtable(v);
    t = (VTable*)(v);
    printf("%I64p = %p\n", &v->fn, t->get(&v->fn));

    t->set(&v->fn, &thisfn);
    printf("%I64p = %p\n", &v->fn, t->get(&v->fn));

    // Clear v from the stack (I think)
    Dummy *d = new Dummy;
    d->fn((void*)0x1337);

    int r = v->fn((void*)0xCAFE);

    printf("Done, %s\n", r==16?"T":"F");
    return 0;
}