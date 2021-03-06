package panda.core;

class Matcher_ExternalImpl implements panda.core.Matcher_External {
    @Override
    public void setText_class_panda$core$$NativePointer_panda$core$String(java.lang.Object nativeHandle, panda.core.String text) {
        ((java.util.regex.Matcher) nativeHandle).reset(
                panda.core.PandaCore.toJavaString(text));
    }

    @Override
    public boolean matches_class_panda$core$$NativePointer_$Rpanda$core$Bit(java.lang.Object nativeHandle) {
        return ((java.util.regex.Matcher) nativeHandle).matches();
    }

    @Override
    public boolean find_class_panda$core$$NativePointer_Int32_$Rpanda$core$Bit(java.lang.Object nativeHandle, int startIndex) {
        return ((java.util.regex.Matcher) nativeHandle).find(startIndex);
    }

    @Override
    public panda.core.String group_class_panda$core$$NativePointer_Int32_$Rpanda$core$String(java.lang.Object nativeHandle, int group) {
        java.lang.String raw = ((java.util.regex.Matcher) nativeHandle).group(group);
        return panda.core.PandaCore.newString(raw != null ? raw : "");
    }

    @Override
    public int groupCount_class_panda$core$$NativePointer_$Rpanda$core$Int32(java.lang.Object nativeHandle) {
        return ((java.util.regex.Matcher) nativeHandle).groupCount() + 1;
    }

    @Override
    public int start_class_panda$core$$NativePointer_$Rpanda$core$Int32(java.lang.Object nativeHandle) {
        return ((java.util.regex.Matcher) nativeHandle).start();
    }

    @Override
    public int end_class_panda$core$$NativePointer_$Rpanda$core$Int32(java.lang.Object nativeHandle) {
        return ((java.util.regex.Matcher) nativeHandle).end();
    }
}