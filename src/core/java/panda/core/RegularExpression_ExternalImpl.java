package panda.core;

class RegularExpression_ExternalImpl implements panda.core.RegularExpression_External {
    @Override
    public panda.core.RegularExpression $clone_$Rpanda$core$RegularExpression(panda.core.RegularExpression self) {
        return panda.core.RegularExpression.createnew$init_panda$core$String(self.$pattern);
    }

    @Override
    public void compile_panda$core$String_Int32(panda.core.RegularExpression self, panda.core.String regex, int flags) {
        int javaFlags = 0;
        if ((flags & panda.core.RegularExpression.$MULTILINE) != 0)
            javaFlags |= java.util.regex.Pattern.MULTILINE;
        self.$nativeHandle = java.util.regex.Pattern.compile(
                PandaCore.toJavaString(regex), javaFlags).matcher("");
    }

    @Override
    public void close(panda.core.RegularExpression self) {
    }
}
