package panda.threads;

import java.util.HashMap;
import java.util.Map;

class Thread_ExternalImpl implements panda.threads.Thread_External {
    Map<java.lang.Thread, panda.threads.Thread> nativeToPanda = 
            new HashMap<java.lang.Thread, panda.threads.Thread>();
    Map<panda.threads.Thread, java.lang.Thread> pandaToNative = 
            new HashMap<panda.threads.Thread, java.lang.Thread>();

    @Override
    public panda.threads.Thread currentThread_class_$Rpanda$threads$Thread() {
        java.lang.Thread current = java.lang.Thread.currentThread();
        panda.threads.Thread result = nativeToPanda.get(current);
        if (result == null) {
            result = panda.threads.Thread.createnew$init();
            nativeToPanda.put(current, result);
            pandaToNative.put(result, current);
        }
        return result;
    }

    @Override
    public void sleep_class_Int32(int millis) {
        try {
            java.lang.Thread.sleep(millis);
        }
        catch (InterruptedException e) {
        }
    }

    @Override
    public void startThread_panda$collections$HashMap$LTpanda$core$Immutable$Cpanda$core$Object$GT(panda.threads.Thread self, panda.collections.HashMap$.$Lpanda$core$Immutable$.$Cpanda$core$Object$R context) {
        java.lang.Thread t = new java.lang.Thread() {
            public void run() {
                Thread.$context.set(context);
                nativeToPanda.put(java.lang.Thread.currentThread(), self);
                pandaToNative.put(self, java.lang.Thread.currentThread());
                try {
                    self.run();
                }
                catch (ThreadDeath e) {
                    throw e;
                }
                catch (Throwable e) {
                    e.printStackTrace();
                    System.exit(1);
                }
                nativeToPanda.remove(java.lang.Thread.currentThread());
                pandaToNative.remove(self);
            }
        };
        t.setDaemon(!self.$preventsExit);
        t.start();
    }

    @Override
    public void threadExit_class() {
        java.lang.Thread.currentThread().stop();
    }
}
