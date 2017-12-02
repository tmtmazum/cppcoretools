

## scoped_timer

```c++
#include <cppcoretools/scoped_timer.h>

using namespace cct;

int my_operation()
{
  scoped_timer timer{"my_operation"};

  .. do some work ..

}  // prints "Operation 'my_operation' took 1s 10us 23ms 41ns"
```

```c++
#include <cppcoretools/scoped_timer.h>

using namespace cct;

int my_operation()
{
  scoped_timer timer{"my_operation", [](auto duration, auto op)
	{
		if(std::chrono::duration_cast<std::chrono::seconds>(duration).count() > 2)
		{
			cct::print("Operation '%s' took larger than 2 seconds", op.c_str());
		}
		else
		{
			cct::print("Operation '%s' ended as expected", op.c_str());
		} 
	}};

  .. do some work ..

}  // prints either "Operation '%s' took larger than 2 seconds" OR "Operation '%s' ended as expected"
```
