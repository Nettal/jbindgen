//
// Created by snownf on 23-12-4.
//

#include "SharedGenerator.h"

namespace jbindgen {

    std::string getAbstractNativeListContent() {
        return "\n"
               "import java.lang.foreign.Arena;\n"
               "import java.lang.foreign.MemorySegment;\n"
               "import java.util.*;\n"
               "import java.util.function.Consumer;\n"
               "import java.util.function.Function;\n"
               "\n"
               "public class AbstractNativeList<T> extends AbstractList<T> implements RandomAccess, Pointer<T> {\n"
               "\n"
               "    protected final MemorySegment ptr;\n"
               "    protected final Function<MemorySegment, T> constructor;\n"
               "    protected final long elementByteSize;\n"
               "\n"
               "    private void ensureValue(MemorySegment ptr, Function<MemorySegment, T> constructor, long sizeof) {\n"
               "        Objects.requireNonNull(ptr);\n"
               "        Objects.requireNonNull(constructor);\n"
               "        if (sizeof <= 0)\n"
               "            throw new IllegalArgumentException(\"Illegal sizeof: \" + sizeof);\n"
               "    }\n"
               "\n"
               "    public AbstractNativeList(MemorySegment ptr, Function<MemorySegment, T> constructor, long elementByteSize) {\n"
               "        this.ptr = ptr;\n"
               "        this.constructor = constructor;\n"
               "        this.elementByteSize = elementByteSize;\n"
               "        ensureValue(ptr, constructor, elementByteSize);\n"
               "    }\n"
               "\n"
               "    public AbstractNativeList(MemorySegment ptr, long length, Function<MemorySegment, T> constructor, long elementByteSize) {\n"
               "        this(ptr.reinterpret(length * elementByteSize), constructor, elementByteSize);\n"
               "    }\n"
               "\n"
               "    public AbstractNativeList(MemorySegment ptr, long length, Arena arena, Consumer<MemorySegment> cleanup, Function<MemorySegment, T> constructor, long elementByteSize) {\n"
               "        this(ptr.reinterpret(length * elementByteSize, arena, cleanup), constructor, elementByteSize);\n"
               "    }\n"
               "\n"
               "    public AbstractNativeList(MemorySegment ptr, Arena arena, Consumer<MemorySegment> cleanup, Function<MemorySegment, T> constructor, long elementByteSize) {\n"
               "        this(ptr.reinterpret(arena, cleanup), constructor, elementByteSize);\n"
               "    }\n"
               "\n"
               "    public AbstractNativeList(Arena arena, long length, Function<MemorySegment, T> constructor, long elementByteSize) {\n"
               "        this(arena.allocate(elementByteSize * length), constructor, elementByteSize);\n"
               "    }\n"
               "\n"
               "    @Override\n"
               "    public T get(int index) {\n"
               "        return constructor.apply(ptr.asSlice(index * elementByteSize, elementByteSize));\n"
               "    }\n"
               "\n"
               "    @Override\n"
               "    public int size() {\n"
               "        return (int) (ptr.byteSize() / elementByteSize);\n"
               "    }\n"
               "\n"
               "    @Override\n"
               "    public void clear() {\n"
               "        ptr.fill((byte) 0);\n"
               "    }\n"
               "\n"
               "    @Override\n"
               "    public AbstractNativeList<T> subList(int fromIndex, int toIndex) {\n"
               "        subListRangeCheck(fromIndex, toIndex, size());\n"
               "        return new AbstractNativeList<>(ptr.asSlice(fromIndex * elementByteSize, (toIndex - fromIndex) * elementByteSize), constructor, elementByteSize);\n"
               "    }\n"
               "\n"
               "    public long getElementByteSize() {\n"
               "        return elementByteSize;\n"
               "    }\n"
               "\n"
               "    public Function<MemorySegment, T> getConstructor() {\n"
               "        return constructor;\n"
               "    }\n"
               "\n"
               "    @Override\n"
               "    public boolean equals(Object o) {\n"
               "        if (o instanceof AbstractNativeList<?> nativeList) return nativeList.ptr.equals(ptr);\n"
               "        return false;\n"
               "    }\n"
               "\n"
               "    @Override\n"
               "    public int hashCode() {\n"
               "        return ptr.hashCode();\n"
               "    }\n"
               "\n"
               "    public List<T> toList() {\n"
               "        return Collections.unmodifiableList(this);\n"
               "    }\n"
               "\n"
               "    @Override\n"
               "    public MemorySegment pointer() {\n"
               "        return ptr;\n"
               "    }\n"
               "\n"
               "    protected static void subListRangeCheck(int fromIndex, int toIndex, int size) {\n"
               "        if (fromIndex < 0)\n"
               "            throw new IndexOutOfBoundsException(\"fromIndex = \" + fromIndex);\n"
               "        if (toIndex > size)\n"
               "            throw new IndexOutOfBoundsException(\"toIndex = \" + toIndex);\n"
               "        if (fromIndex > toIndex)\n"
               "            throw new IllegalArgumentException(\"fromIndex(\" + fromIndex +\n"
               "                    \") > toIndex(\" + toIndex + \")\");\n"
               "    }\n"
               "}\n";
    }

    std::string getPointerContent() {
        return "\n"
               "import java.lang.foreign.MemorySegment;\n"
               "\n"
               "public interface Pointer<P>{\n"
               "    MemorySegment pointer();\n"
               "}\n";
    }

    std::string getFunctionUtilsContent() {
        return "\n"
               "import java.lang.foreign.*;\n"
               "import java.lang.invoke.MethodHandle;\n"
               "import java.lang.invoke.MethodHandles;\n"
               "import java.util.Objects;\n"
               "import java.util.Optional;\n"
               "\n"
               "public class FunctionUtils {\n"
               "    public static class InvokeException extends RuntimeException {\n"
               "        public InvokeException(Throwable cause) {\n"
               "            super(cause);\n"
               "        }\n"
               "    }\n"
               "\n"
               "    public static class SymbolNotFound extends RuntimeException {\n"
               "        public SymbolNotFound(String cause) {\n"
               "            super(cause);\n"
               "        }\n"
               "\n"
               "        public SymbolNotFound(Throwable t) {\n"
               "            super(t);\n"
               "        }\n"
               "\n"
               "        public SymbolNotFound() {\n"
               "        }\n"
               "    }\n"
               "\n"
               "    private FunctionUtils() {\n"
               "        throw new UnsupportedOperationException();\n"
               "    }\n"
               "\n"
               "    public static MemorySegment toMemorySegment(Arena arena, MethodHandle methodHandle, FunctionDescriptor functionDescriptor) {\n"
               "        return Linker.nativeLinker().upcallStub(methodHandle, functionDescriptor, arena);\n"
               "    }\n"
               "\n"
               "    public static MemorySegment toMemorySegment(MethodHandles.Lookup lookup, Arena arena, FunctionDescriptor functionDescriptor, Object function, String functionName) {\n"
               "        var handle = toMethodHandle(lookup, functionDescriptor, function, functionName);\n"
               "        return toMemorySegment(arena, handle, functionDescriptor);\n"
               "    }\n"
               "\n"
               "    public static MethodHandle toMethodHandle(MethodHandles.Lookup lookup, FunctionDescriptor functionDescriptor, Object function, String functionName) {\n"
               "        try {\n"
               "            return lookup.findVirtual(function.getClass(), functionName, functionDescriptor.toMethodType()).bindTo(function);\n"
               "        } catch (NoSuchMethodException | IllegalAccessException e) {\n"
               "            throw new RuntimeException(e);\n"
               "        }\n"
               "    }\n"
               "\n"
               "    public static Optional<MethodHandle> toMethodHandle(SymbolLookup lookup, String functionName, FunctionDescriptor functionDescriptor, boolean critical) {\n"
               "        return Objects.requireNonNull(lookup).find(Objects.requireNonNull(functionName)).map(ms -> critical ?\n"
               "                Linker.nativeLinker().downcallHandle(ms, functionDescriptor, Linker.Option.critical(true))\n"
               "                : Linker.nativeLinker().downcallHandle(ms, functionDescriptor));\n"
               "    }\n"
               "\n"
               "    public static Optional<MethodHandle> toMethodHandle(MemorySegment memorySegment, FunctionDescriptor functionDescriptor, boolean critical) {\n"
               "        return toMethodHandle(Optional.ofNullable(memorySegment), functionDescriptor, critical);\n"
               "    }\n"
               "\n"
               "    public static Optional<MethodHandle> toMethodHandle(Optional<MemorySegment> memorySegment, FunctionDescriptor functionDescriptor, boolean critical) {\n"
               "        return memorySegment.map(ms -> critical ?\n"
               "                Linker.nativeLinker().downcallHandle(ms, functionDescriptor, Linker.Option.critical(true))\n"
               "                : Linker.nativeLinker().downcallHandle(ms, functionDescriptor));\n"
               "    }\n"
               "}\n";
    }

    std::string getNListContent() {
        return "\n"
               "import java.lang.foreign.Arena;\n"
               "import java.lang.foreign.MemorySegment;\n"
               "import java.util.function.Consumer;\n"
               "import java.util.function.Function;\n"
               "\n"
               "public class NList<E extends Pointer<E>> extends AbstractNativeList<E> {\n"
               "    public NList(MemorySegment ptr, Function<MemorySegment, E> constructor, long elementByteSize) {\n"
               "        super(ptr, constructor, elementByteSize);\n"
               "    }\n"
               "\n"
               "    public NList(MemorySegment ptr, long length, Function<MemorySegment, E> constructor, long elementByteSize) {\n"
               "        super(ptr, length, constructor, elementByteSize);\n"
               "    }\n"
               "\n"
               "    public NList(MemorySegment ptr, long length, Arena arena, Consumer<MemorySegment> cleanup, Function<MemorySegment, E> constructor, long elementByteSize) {\n"
               "        super(ptr, length, arena, cleanup, constructor, elementByteSize);\n"
               "    }\n"
               "\n"
               "    public NList(MemorySegment ptr, Arena arena, Consumer<MemorySegment> cleanup, Function<MemorySegment, E> constructor, long elementByteSize) {\n"
               "        super(ptr, arena, cleanup, constructor, elementByteSize);\n"
               "    }\n"
               "\n"
               "    public NList(Arena arena, long length, Function<MemorySegment, E> constructor, long elementByteSize) {\n"
               "        super(arena, length, constructor, elementByteSize);\n"
               "    }\n"
               "\n"
               "    /**\n"
               "     * @param index   index of the element to replace\n"
               "     * @param element element to be stored at the specified position\n"
               "     * @return the element\n"
               "     */\n"
               "    @Override\n"
               "    public E set(int index, E element) {\n"
               "        if (element.pointer().byteSize() != elementByteSize) throw new IllegalArgumentException();\n"
               "        MemorySegment.copy(element.pointer(), 0, ptr, index * elementByteSize, elementByteSize);\n"
               "        return element;\n"
               "    }\n"
               "\n"
               "    @Override\n"
               "    public String toString() {\n"
               "        return pointer().byteSize() % elementByteSize == 0 ? super.toString() : \"NativeList{ptr: \" + ptr + \"}\";\n"
               "    }\n"
               "}\n";
    }

    std::string getNPtrListContent() {
        return "\n"
               "import java.lang.foreign.Arena;\n"
               "import java.lang.foreign.MemorySegment;\n"
               "import java.lang.foreign.ValueLayout;\n"
               "import java.util.Arrays;\n"
               "import java.util.Collection;\n"
               "import java.util.function.Consumer;\n"
               "import java.util.function.Function;\n"
               "\n"
               "public class NPtrList<T extends Pointer<T>> extends NList<T> {\n"
               "    private final long[] elementsByteSize;\n"
               "\n"
               "    public NPtrList(MemorySegment ptr, Function<MemorySegment, T> constructor, long elementByteSize, long length) {\n"
               "        this(ptr.reinterpret(length * ValueLayout.ADDRESS.byteSize()), constructor, elementByteSize);\n"
               "    }\n"
               "\n"
               "    public NPtrList(MemorySegment ptr, Function<MemorySegment, T> constructor, long[] elementsByteSize) {\n"
               "        super(ptr.reinterpret(ValueLayout.ADDRESS.byteSize() * elementsByteSize.length), constructor, ValueLayout.ADDRESS.byteSize());\n"
               "        this.elementsByteSize = elementsByteSize;\n"
               "    }\n"
               "\n"
               "    public NPtrList(MemorySegment ptr, Arena arena, Consumer<MemorySegment> cleanup, Function<MemorySegment, T> constructor, long elementByteSize, long length) {\n"
               "        this(ptr.reinterpret(length * ValueLayout.ADDRESS.byteSize(), arena, cleanup), constructor, elementByteSize);\n"
               "    }\n"
               "\n"
               "    public NPtrList(MemorySegment ptr, Arena arena, Consumer<MemorySegment> cleanup, Function<MemorySegment, T> constructor, long[] elementsByteSize) {\n"
               "        this(ptr.reinterpret(elementsByteSize.length * ValueLayout.ADDRESS.byteSize(), arena, cleanup), constructor, elementsByteSize);\n"
               "    }\n"
               "\n"
               "    public NPtrList(MemorySegment ptr, Arena arena, Consumer<MemorySegment> cleanup, Function<MemorySegment, T> constructor, long elementByteSize) {\n"
               "        this(ptr.reinterpret(arena, cleanup), constructor, elementByteSize);\n"
               "    }\n"
               "\n"
               "    public NPtrList(Arena arena, Function<MemorySegment, T> constructor, T[] t) {\n"
               "        this(arena.allocate(ValueLayout.ADDRESS.byteSize() * t.length), constructor, Arrays.stream(t).mapToLong(v -> v.pointer().byteSize()).toArray());\n"
               "        for (int i = 0; i < t.length; i++) {\n"
               "            ptr.setAtIndex(ValueLayout.ADDRESS, i, t[i].pointer());\n"
               "        }\n"
               "    }\n"
               "\n"
               "    public NPtrList(Arena arena, Function<MemorySegment, T> constructor, Collection<T> t) {\n"
               "        this(arena.allocate(ValueLayout.ADDRESS.byteSize() * t.size()), constructor, t.stream().mapToLong(v -> v.pointer().byteSize()).toArray());\n"
               "        int i = 0;\n"
               "        for (T t1 : t) {\n"
               "            ptr.setAtIndex(ValueLayout.ADDRESS, i, t1.pointer());\n"
               "            i++;\n"
               "        }\n"
               "    }\n"
               "\n"
               "    public NPtrList(MemorySegment ptr, Function<MemorySegment, T> constructor, long elementByteSize) {\n"
               "        super(ptr, constructor, ValueLayout.ADDRESS.byteSize());\n"
               "        elementsByteSize = new long[]{elementByteSize};\n"
               "    }\n"
               "\n"
               "\n"
               "    @Override\n"
               "    public T get(int index) {\n"
               "        return constructor.apply(ptr.getAtIndex(ValueLayout.ADDRESS, index).reinterpret(elementsByteSize.length > 1 ? elementsByteSize[index] : elementsByteSize[0]));\n"
               "    }\n"
               "\n"
               "    @Override\n"
               "    public T set(int index, T value) {\n"
               "        ptr.setAtIndex(ValueLayout.ADDRESS, index, value.pointer());\n"
               "        return value;\n"
               "    }\n"
               "\n"
               "    @Override\n"
               "    public void clear() {\n"
               "        throw new UnsupportedOperationException();\n"
               "    }\n"
               "}\n";
    }

    std::string getValueContent() {
        return "\n"
               "public interface Value<T>{\n"
               "    T value();\n"
               "}\n";
    }

    std::string getVListContent() {
        return "\n"
               "import java.lang.foreign.Arena;\n"
               "import java.lang.foreign.MemorySegment;\n"
               "import java.lang.foreign.ValueLayout;\n"
               "import java.util.Collection;\n"
               "import java.util.List;\n"
               "import java.util.Objects;\n"
               "import java.util.function.BiConsumer;\n"
               "import java.util.function.Consumer;\n"
               "import java.util.function.Function;\n"
               "\n"
               "public class VList<T extends Value<V>, V> extends AbstractNativeList<T> {\n"
               "    public VList(MemorySegment ptr, Function<MemorySegment, T> constructor, long byteSize) {\n"
               "        super(ptr, constructor, byteSize);\n"
               "    }\n"
               "\n"
               "    public VList(MemorySegment ptr, long length, Function<MemorySegment, T> constructor, long byteSize) {\n"
               "        super(ptr, length, constructor, byteSize);\n"
               "    }\n"
               "\n"
               "    public VList(MemorySegment ptr, long length, Arena arena, Consumer<MemorySegment> cleanup, Function<MemorySegment, T> constructor, long byteSize) {\n"
               "        super(ptr, length, arena, cleanup, constructor, byteSize);\n"
               "    }\n"
               "\n"
               "    public VList(Arena arena, long length, Function<MemorySegment, T> constructor, BiConsumer<Long, VList<T, V>> creator, long byteSize) {\n"
               "        this(arena.allocate(byteSize * length, 4), length, constructor, byteSize);\n"
               "        for (int i = 0; i < length; i++) {\n"
               "            creator.accept((long) i, subList(i, i));\n"
               "        }\n"
               "    }\n"
               "\n"
               "    public VList(Arena arena, long length, Function<MemorySegment, T> constructor, long byteSize) {\n"
               "        super(arena.allocate(length * byteSize), constructor, byteSize);\n"
               "    }\n"
               "\n"
               "    public VList(Arena arena, T[] objs, Function<MemorySegment, T> constructor, long byteSize) {\n"
               "        super(arena.allocate(objs.length * byteSize), constructor, byteSize);\n"
               "        long index = 0;\n"
               "        for (T obj : objs) {\n"
               "            setAny(pointer(), index, obj.value());\n"
               "            index++;\n"
               "        }\n"
               "    }\n"
               "\n"
               "    public VList(Arena arena, List<T> objs, Function<MemorySegment, T> constructor, long byteSize) {\n"
               "        super(arena.allocate(objs.size() * byteSize), constructor, byteSize);\n"
               "        long index = 0;\n"
               "        for (T obj : objs) {\n"
               "            setAny(pointer(), index, obj.value());\n"
               "            index++;\n"
               "        }\n"
               "    }\n"
               "\n"
               "    public VList(Arena arena, V[] objs, Function<MemorySegment, T> constructor, long byteSize) {\n"
               "        super(arena.allocate(objs.length * byteSize), constructor, byteSize);\n"
               "        long index = 0;\n"
               "        for (V obj : objs) {\n"
               "            setAny(pointer(), index, obj);\n"
               "            index++;\n"
               "        }\n"
               "    }\n"
               "\n"
               "    public VList(Arena arena, Collection<V> objs, Function<MemorySegment, T> constructor, long byteSize) {\n"
               "        super(arena.allocate(objs.size() * byteSize), constructor, byteSize);\n"
               "        long index = 0;\n"
               "        for (V obj : objs) {\n"
               "            setAny(pointer(), index, obj);\n"
               "            index++;\n"
               "        }\n"
               "    }\n"
               "\n"
               "    private static void setAny(MemorySegment ptr, long index, Object object) {\n"
               "        switch (object) {\n"
               "            case MemorySegment m -> ptr.setAtIndex(ValueLayout.ADDRESS, index, m);\n"
               "            case Integer i -> ptr.setAtIndex(ValueLayout.JAVA_INT, index, i);\n"
               "            case Long l -> ptr.setAtIndex(ValueLayout.JAVA_LONG, index, l);\n"
               "            case Float f -> ptr.setAtIndex(ValueLayout.JAVA_FLOAT, index, f);\n"
               "            case Double d -> ptr.setAtIndex(ValueLayout.JAVA_DOUBLE, index, d);\n"
               "            case Byte b -> ptr.setAtIndex(ValueLayout.JAVA_BYTE, index, b);\n"
               "            case Boolean b -> ptr.setAtIndex(ValueLayout.JAVA_BOOLEAN, index, b);\n"
               "            case Character c -> ptr.setAtIndex(ValueLayout.JAVA_CHAR, index, c);\n"
               "            case Short s -> ptr.setAtIndex(ValueLayout.JAVA_SHORT, index, s);\n"
               "            default -> throw new IllegalArgumentException(\"Unexpected value: \" + object);\n"
               "        }\n"
               "    }\n"
               "\n"
               "\n"
               "    @Override\n"
               "    public T get(int index) {\n"
               "        return constructor.apply(ptr.getAtIndex(ValueLayout.ADDRESS, index));\n"
               "    }\n"
               "\n"
               "    public V getValue(int index) {\n"
               "        return constructor.apply(ptr.getAtIndex(ValueLayout.ADDRESS, index)).value();\n"
               "    }\n"
               "\n"
               "    @Override\n"
               "    public T set(int index, T element) {\n"
               "        setAny(pointer(), index, element.value());\n"
               "        return element;\n"
               "    }\n"
               "\n"
               "    public V setValue(int index, V element) {\n"
               "        setAny(pointer(), index, element);\n"
               "        return element;\n"
               "    }\n"
               "\n"
               "    @Override\n"
               "    public VList<T, V> subList(int fromIndex, int toIndex) {\n"
               "        subListRangeCheck(fromIndex, toIndex, size());\n"
               "        return new VList<>(ptr.asSlice(fromIndex * elementByteSize, (toIndex - fromIndex) * elementByteSize), constructor, elementByteSize);\n"
               "    }\n"
               "\n"
               "    @Override\n"
               "    public String toString() {\n"
               "        return pointer().byteSize() % elementByteSize == 0 ? super.toString() : \"ObjectList{ptr: \" + ptr + \"}\";\n"
               "    }\n"
               "}\n";
    }

    std::string getValueContent(std::string className, std::string baseObjectType, std::string valueLayout,
                                std::string basePrimitiveType, std::string sharedVListPackageName,
                                std::string sharedValuePackageName) {
        return std::vformat(
                "\n"
                "import {4};\n"
                "import {5};\n"
                "\n"
                "import java.lang.foreign.Arena;\n"
                "import java.lang.foreign.MemorySegment;\n"
                "import java.lang.foreign.ValueLayout;\n"
                "import java.util.Collection;\n"
                "import java.util.List;\n"
                "import java.util.function.Consumer;\n"
                "import java.util.function.Function;\n"
                "\n"
                "public class {0} implements Value<{1}> {{\n"
                "    public static final long BYTE_SIZE = {2}.byteSize();\n"
                "\n"
                "    public static VList<{0}, {1}> list(MemorySegment ptr) {{\n"
                "        return new VList<>(ptr, {0}::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    public static VList<{0}, {1}> list(MemorySegment ptr, long length) {{\n"
                "        return new VList<>(ptr, length, {0}::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    public static VList<{0}, {1}> list(MemorySegment ptr, long length, Arena arena, Consumer<MemorySegment> cleanup) {{\n"
                "        return new VList<>(ptr, length, arena, cleanup, {0}::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    public static VList<{0}, {1}> list(Arena arena, long length) {{\n"
                "        return new VList<>(arena, length, {0}::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    public static VList<{0}, {1}> list(Arena arena, {1}[] c) {{\n"
                "        return new VList<>(arena, c, {0}::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    public static VList<{0}, {1}> list(Arena arena, Collection<{1}> c) {{\n"
                "        return new VList<>(arena, c.size(), {0}::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    public static VList<{0}, {1}> list(Arena arena, {0}[] c) {{\n"
                "        return new VList<>(arena, c, (Function<MemorySegment, {0}>) {0}::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    public static VList<{0}, {1}> list(Arena arena, List<{0}> c) {{\n"
                "        return new VList<>(arena, c.size(), {0}::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    private final {3} value;\n"
                "\n"
                "    public {0}(MemorySegment ptr) {{\n"
                "        this.value = ptr.get({2}, 0);\n"
                "    }}\n"
                "\n"
                "    public {0}({3} value) {{\n"
                "        this.value = value;\n"
                "    }}\n"
                "\n"
                "    public {0}(Value<{1}> value) {{\n"
                "        this.value = value.value();\n"
                "    }}\n"
                "\n"
                "    @Override\n"
                "    public {1} value() {{\n"
                "        return value;\n"
                "    }}\n"
                "\n"
                "    @Override\n"
                "    public String toString() {{\n"
                "        return String.valueOf(value);\n"
                "    }}\n"
                "}}\n",
                std::make_format_args(className, baseObjectType,
                                      valueLayout, basePrimitiveType,
                                      sharedValuePackageName, sharedVListPackageName));
    }

    std::string getNativeContent(std::string className, std::string baseObjectType, std::string valueLayout,
                                 std::string basePrimitiveType, std::string sharedPointerPackageName,
                                 std::string sharedValuePackageName, std::string sharedNListPackageName) {
        return std::vformat(
                "\n"
                "import {4};\n"
                "import {5};\n"
                "import {6};\n"
                "\n"
                "import java.lang.foreign.Arena;\n"
                "import java.lang.foreign.MemorySegment;\n"
                "import java.lang.foreign.ValueLayout;\n"
                "import java.util.function.Consumer;\n"
                "\n"
                "public class {0} implements Pointer<{0}>, Value<{1}> {{\n"
                "    public static final long BYTE_SIZE = {2}.byteSize();\n"
                "\n"
                "    public static NList<{0}> list(MemorySegment ptr) {{\n"
                "        return new NList<>(ptr, {0}::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    public static NList<{0}> list(MemorySegment ptr, long length) {{\n"
                "        return new NList<>(ptr, length, {0}::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    public static NList<{0}> list(MemorySegment ptr, long length, Arena arena, Consumer<MemorySegment> cleanup) {{\n"
                "        return new NList<>(ptr, length, arena, cleanup, {0}::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    public static NList<{0}> list(Arena arena, long length) {{\n"
                "        return new NList<>(arena, length, {0}::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    public static NList<{0}> list(Arena arena, {3}[] l) {{\n"
                "        return new NList<>(arena.allocateFrom({2}, l), l.length, {0}::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    private final MemorySegment ptr;\n"
                "\n"
                "    public {0}(MemorySegment ptr) {{\n"
                "        this.ptr = ptr;\n"
                "    }}\n"
                "\n"
                "    public {0}(MemorySegment ptr, Arena arena, Consumer<MemorySegment> cleanup) {{\n"
                "        this.ptr = ptr.reinterpret(BYTE_SIZE, arena, cleanup);\n"
                "    }}\n"
                "\n"
                "    public {0}(Arena arena) {{\n"
                "        ptr = arena.allocate({2});\n"
                "    }}\n"
                "\n"
                "    public {0}(Arena arena, {3} l) {{\n"
                "        ptr = arena.allocateFrom({2}, l);\n"
                "    }}\n"
                "\n"
                "    public {0} fill(int value) {{\n"
                "        ptr.fill((byte) value);\n"
                "        return this;\n"
                "    }}\n"
                "\n"
                "    @Override\n"
                "    public {1} value() {{\n"
                "        return get();\n"
                "    }}\n"
                "\n"
                "    public {3} get() {{\n"
                "        return ptr.get({2}, 0);\n"
                "    }}\n"
                "\n"
                "    public {0} set({3} value) {{\n"
                "        ptr.setAtIndex({2}, 0, value);\n"
                "        return this;\n"
                "    }}\n"
                "\n"
                "    @Override\n"
                "    public MemorySegment pointer() {{\n"
                "        return ptr;\n"
                "    }}\n"
                "\n"
                "    public long byteSize() {{\n"
                "        return ptr.byteSize();\n"
                "    }}\n"
                "\n"
                "    @Override\n"
                "    public String toString() {{\n"
                "        return MemorySegment.NULL.address() != ptr.address() && ptr.byteSize() >= BYTE_SIZE\n"
                "                ? String.valueOf(get())\n"
                "                : \"NativeShort{{ptr: \" + ptr + \"}}\";\n"
                "    }}\n"
                "}}\n", std::make_format_args(className, baseObjectType, valueLayout,
                                              basePrimitiveType, sharedNListPackageName, sharedPointerPackageName,
                                              sharedValuePackageName));
    }

    std::string getNPointerWithClassName(std::string className, std::string sharedPointerPackageName,
                                         std::string sharedValuePackageName, std::string sharedNListPackageName) {
        return std::vformat(
                "\n"
                "import {1};\n"
                "import {2};\n"
                "import {3};\n"
                "\n"
                "import java.lang.foreign.Arena;\n"
                "import java.lang.foreign.MemorySegment;\n"
                "import java.lang.foreign.ValueLayout;\n"
                "import java.util.Arrays;\n"
                "import java.util.function.Consumer;\n"
                "\n"
                "public class {0} implements Pointer<{0}>, Value<MemorySegment> {{\n"
                "    public static final long BYTE_SIZE = ValueLayout.ADDRESS.byteSize();\n"
                "\n"
                "    public static NList<{0}> list(MemorySegment ptr) {{\n"
                "        return new NList<>(ptr, NPointer::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    public static NList<{0}> list(MemorySegment ptr, long length) {{\n"
                "        return new NList<>(ptr, length, NPointer::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    public static NList<{0}> list(MemorySegment ptr, long length, Arena arena, Consumer<MemorySegment> cleanup) {{\n"
                "        return new NList<>(ptr, length, arena, cleanup, NPointer::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    public static NList<{0}> list(Arena arena, long length) {{\n"
                "        return new NList<>(arena, length, NPointer::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    public static NList<{0}> list(Arena arena, MemorySegment[] l) {{\n"
                "        return new NList<>(arena.allocateFrom(ValueLayout.JAVA_LONG, Arrays.stream(l).mapToLong(MemorySegment::address).toArray()), l.length, NPointer::new, BYTE_SIZE);\n"
                "    }}\n"
                "\n"
                "    private final MemorySegment ptr;\n"
                "\n"
                "    public NPointer(MemorySegment ptr) {{\n"
                "        this.ptr = ptr;\n"
                "    }}\n"
                "\n"
                "    public NPointer(MemorySegment ptr, Arena arena, Consumer<MemorySegment> cleanup) {{\n"
                "        this.ptr = ptr.reinterpret(BYTE_SIZE, arena, cleanup);\n"
                "    }}\n"
                "\n"
                "    public NPointer(Arena arena) {{\n"
                "        ptr = arena.allocate(ValueLayout.ADDRESS);\n"
                "    }}\n"
                "\n"
                "    public NPointer(Arena arena, MemorySegment l) {{\n"
                "        ptr = arena.allocateFrom(ValueLayout.ADDRESS, l);\n"
                "    }}\n"
                "\n"
                "    public NPointer fill(int value) {{\n"
                "        ptr.fill((byte) value);\n"
                "        return this;\n"
                "    }}\n"
                "\n"
                "    @Override\n"
                "    public MemorySegment value() {{\n"
                "        return get();\n"
                "    }}\n"
                "\n"
                "    public MemorySegment get() {{\n"
                "        return ptr.get(ValueLayout.ADDRESS, 0);\n"
                "    }}\n"
                "\n"
                "    public NPointer set(MemorySegment value) {{\n"
                "        ptr.setAtIndex(ValueLayout.ADDRESS, 0, value);\n"
                "        return this;\n"
                "    }}\n"
                "\n"
                "    @Override\n"
                "    public MemorySegment pointer() {{\n"
                "        return ptr;\n"
                "    }}\n"
                "\n"
                "    public long byteSize() {{\n"
                "        return BYTE_SIZE;\n"
                "    }}\n"
                "\n"
                "    @Override\n"
                "    public String toString() {{\n"
                "        return MemorySegment.NULL.address() != ptr.address() && ptr.byteSize() >= BYTE_SIZE\n"
                "                ? \"NativeAddress{{\" + \"ptr=\" + ptr + \", value=\" + get() + '}}'\n"
                "                : \"NativeAddress{{ptr: \" + ptr + \"}}\";\n"
                "    }}\n"
                "}}\n", std::make_format_args(className, sharedNListPackageName, sharedPointerPackageName,
                                              sharedValuePackageName));
    }
} // jbindgen