# cppcoretools
A set of common tools written in modern c++ focusing heavily on RAII and C++11/14/17 features.  

## Overview of Features

- `scoped_redirect`  
		Redirects print output to a file temporarily within a scope
- `scoped_failure_handler`  
		Overrides current operation to perform on expectation failure, within a scope
- `CCT_CHECK`  
		Performs a runtime-check and executes the failure handler on failure
- `to_string(std::chrono::duration<..> )`  
		Converts a `std::chrono::duration` type to a readable string of the format: 1hr(s) 2min(s) 3s 4ms 5us 6ns
- `scoped_timer`  
		Measures time elapsed in a scope and prints it

- `unique_file`  
		RAII wrapper around FILE*

# Usage

## scoped_redirect

Redirects printing output to a file for a fixed scope

```c++
#include <cppcoretools/print.h>

using namespace cct;

int nested_operation()
{
	// need to print to console for debugging here
	scoped_redirect redirect{stdout};

	println("Running nested op.."); // logs to console
}

int some_operation()
{
 // need to output to file
	scoped_redirect redirect{"out_file.txt"};

	nested_operation();

	println("Hello World!"); // outputs "Hello World!" to out_file.txt
}
```


## scoped_timer

- measures the time elapsed in a scope and logs it
- can specify custom function to process duration elapsed
- makes use of std::chrono::duration and time_point classes (C++11)

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


