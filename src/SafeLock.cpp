#include "SafeLock.hpp"

#ifdef TESTING
#include <catch2/catch_test_macros.hpp>
#endif

using namespace AppCore;

SafeLock::~SafeLock()
{
    while(is_locked>0)
        unlock();
}

void SafeLock::lock() noexcept
{
    _internal.lock();
    is_locked++;
}

void SafeLock::lockTimeout(size_t timeout_ms)
{
    if(!_internal.try_lock_for(std::chrono::milliseconds(timeout_ms))){
        throw SafeLockTimeoutException();
    }
    else is_locked++; 
}

void SafeLock::unlock()
{
    if(is_locked>0){
        _internal.unlock();
        is_locked--;
    }
}

#ifdef TESTING
//Safe Lock
#include <thread>
SCENARIO("Safe Lock Recursive Calls","[SafeLock]"){
    GIVEN("A SafeLock"){
        SafeLock l;
        THEN("The lock starts unlocked"){
            REQUIRE(!l.isLocked());
        }
        WHEN("Lock is locked"){
            l.lock();
            AND_THEN("Lock counter is 1"){
                REQUIRE(l.getIsLockedCounter()==1);
                AND_WHEN("Another thread attempts to lock"){
                    bool protected_var = false;
                    std::thread t([&protected_var,&l]{l.lock(); protected_var=true;});
                    AND_THEN("The thread cannot lock until the lock is unlocked"){
                        REQUIRE(protected_var==false);
                        l.unlock();
                        if(t.joinable())t.join();
                        REQUIRE(protected_var==true);
                    }
                }
            }
            AND_WHEN("Lock is unlocked"){
                l.unlock();
                AND_THEN("Lock counter is 0"){
                    REQUIRE(l.getIsLockedCounter()==0);
                }
            }
        }
        WHEN("The lock is locked recursively"){
            l.lock();
            l.lock();
            AND_THEN("Lock counter is 2"){
                REQUIRE(l.getIsLockedCounter()==2);
            }
            AND_WHEN("Lock is unlocked once"){
                l.unlock();
                AND_THEN("Lock counter is 1"){
                    REQUIRE(l.getIsLockedCounter()==1);
                }
            }
            AND_WHEN("Lock is unlocked twice"){
                l.unlock();
                l.unlock();
                AND_THEN("Lock counter is 0"){
                    REQUIRE(l.getIsLockedCounter()==0);
                }
                AND_THEN("Lock is not locked"){
                    REQUIRE(!l.isLocked());
                }
            }
        }
    }
}

#endif