#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "filesystem.h"
#include "interrupt.h"
#include "RTC.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}
	
	
	return result;
}

/* Divide_By_Zero Test - Example
 * 
 * Raises Exceptions 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Divide by zero exception
 * Files: x86_desc.h/S
 */
int Divide_By_Zero_Test(){
	int result = FAIL;
	puts("Exception Test: Divide by Zero"); // if the function completes without printing the exception message, FAIL
	
	/* performing the error*/
	int ans;
	ans = 1/0;

	puts("If youre reading this its too late"); /*Fail message*/
	return result;
}


/* Page_NULL Test - Example
 * 
 * Raises Exceptions 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Page fault null dereference
 * Files: x86_desc.h/S
 */
int Page_NULL_Test()
{
	/* performing the error*/
	int* t = (int*) 0;
	int a = *t;
	return FAIL;
}

/* 
 * System Call Test - Example
 * 
 * Calls system call
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */
int sys_call_test() {
	__asm__("int $0x80");
	return FAIL;
}

/* Page_Vid_Test_END Test - Example
 * 
 * Test Paging with char dereference
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */
int Page_Vid_Test_END() {
	int * t = (int*)(0xB8000 + 8);
	int a;
	a = *t;
	return PASS;
}

/* Page_Vid_Test_START Test - Example
 * 
 * Test Paging with char dereference
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 */
int Page_Vid_Test_START() {
	int * t = (int*)(0xB8000);
	int a;
	a = *t;
	return PASS;
}

/* Page_invalid_test Test - Example
 * 
 * Exception handling
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: causes invalid dereference error
 */
int Page_invalid_test() {
	int *t = (int*)(0xB8);
	int a;
	a = *t;
	return FAIL;
}




// add more tests here

/* Checkpoint 2 tests */
/* Directory Read Test - Example
 * 
 * Testing directory read
 * Inputs: None
 * Outputs: PASS/FAIL
 */
int directory_read_test() {
	test_dir_read();
	return PASS;
}

/* File Read Test - Example
 * 
 * Testing file read
 * Inputs: None
 * Outputs: PASS/FAIL
 */
int file_read_test() {
	test_file_read((uint8_t*) "frame1.txt", 170);
	return PASS;
}

/* EXE File Read Test - Example
 * 
 * Testing file read
 * Inputs: None
 * Outputs: PASS/FAIL
 */
int exe_file_read_test() {
	test_file_read((uint8_t*) "hello", 6000);
	return PASS;
}

/* File Read Overflow Test - Example
 * 
 * Testing file read when requested size is too large
 * Inputs: None
 * Outputs: PASS/FAIL
 */
int file_read_overflow_test() {
	test_file_read((uint8_t*) "verylargetextwithverylongname.txt", 7000);
	return PASS;
}

/* Terminal Read Write Test - Example
 * 
 * Testing terminal read/write functionality
 * Inputs: None
 * Outputs: PASS/FAIL
 */
int terminal_rw_test() {
	test_rw();
	return PASS;
}

/* Terminal RTC Test - Example
 * 
 * Testing rtc functionality
 * Inputs: None
 * Outputs: PASS/FAIL
 */
int RTC_test() {
	rtc_test_func(4);
	return PASS;
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//Checkpoint 1
	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("div_by_zero", Divide_By_Zero_Test())
	//TEST_OUTPUT("sys_call", sys_call_test());
	//TEST_OUTPUT("null_dereference", Page_NULL_Test());
	//TEST_OUTPUT("char_dereference1", Page_Vid_Test_START());
	//TEST_OUTPUT("char_dereference2", Page_Vid_Test_END());
	//TEST_OUTPUT("invalid_dereference", Page_invalid_test());

	//Checkpoint 2
	//TEST_OUTPUT("test read/write terminal", terminal_rw_test());
	//TEST_OUTPUT("test dir read", directory_read_test());
	//TEST_OUTPUT("test file read", file_read_test());
	//TEST_OUTPUT("test file overflow read", file_read_overflow_test());
	//TEST_OUTPUT("Testing RTC", RTC_test());
	//TEST_OUTPUT("Testing EXE Read", exe_file_read_test());
}
