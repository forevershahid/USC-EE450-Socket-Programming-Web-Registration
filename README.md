# USC-EE450-Socket-Programming-Web-Registration
EE450 USC Computer Networks Project

FILES INCLUDED:

	1.	client.c
	2.	serverM.c
	3.	serverC.c
	4.	serverCS.c
	5.	serverEE.c
	6.	Makefile
	7.	Readme.txt	

Note:	Executables are not included as instructed by the Problem Statement.
		'Makeall' command in the terminal generates the executables required.

Note:	Please read the NOTEs as they contain vital information of the overall flow of the project. 

-------------------------------------------------------------------------------

What I have done in the Project:

1. I have NOT IMPLEMENTED the optional Part.

2. Code Files and their Description:
	
	client.c:

			In the client.c program file, I have taken inputs of 'Username' & 'Password' as STRING type.

			They are then compared with the CONSTRAINTS as prescribed by the Problem Statement. 

			1.Username has to be of lower case and 5-50 chars long. Failing which the client will SHUT OFF before the program even starts.
			2.Password has to be between 5-50 chars long. Failing which will cause the program to SHUT OFF again.

			The Username and Password combo are sent to the Main server for Authentication.

			The Result of Authentication is received as:

			1.	"PASS"				 :stating the fact that the Username and Password combination are correct and it is encoded as '0' for further processes.
			2.	"FAIL_NO_USER"		 :stating the fact that the Username was wrong and it is encoded as '1' for further process. The number of tries is reduced by 1 in this case.
			3.	"FAIL_PASS_NO_MATCH" :stating the fact that the Password was wrong and it is then encoded as '2' for further process. The number of tries is reduced by 1 in this case.

			If all 3 attempts have completed with unsuccessful Authentication, the client SHUTS OFF.

			If the user has passed the Authntication, We proceed to the Queries.

			Coursecode and Category are received as STRINGS, The requested 'Credit' information is received as INTEGER and the other Queries as STRINGS from the Main server. 

			The Information of the Query requested is then Displayed. 

			NOTE 1:	The request for displaying the credit of a course being an INTEGER, is received seperately from the servers by comparing the CATEGORY of the requested Query.

			NOTE 2:	Instead of continuing directly to another Query session, the user is given an option to Quit, where he/she can enter '1' to request another Query and '0' for Quitting. (Request to implement this feature was obtained through mail).

	
	serverM.c:

			The Main server firstly receives the Username and Password sent by the client. 

			The Password and Username are then encrypted as instructed by the problem statement. I have done it using ASCII values, after checking each character with the function:

			isalnum(); This function gives us if a character is alphabet or number, as we didnt need to encrypt special characters.

			The encrtpted Username (euser) and encrypted Password (epass) is sent as UDP Datagram to serverC, and the result is received as a STRING.

			The coursecode and Category are then received from the client as STRINGS.

			The first two letters in the coursecode (EE or CS) are found and encoded as 'EE' is 1 and 'CS' is 2,If the Courses are not from 'CS' or 'EE' then error message is desplayed. 

			The Query Request is sent to Appropriate servers and the Information is received as STRINGS, 'Credit' Information is again received as INTEGER.

			The information is Sent to Client.

			NOTE: Choice is received here 
			0 - Shuts off SERVER M
			1 - Ready for another Query.

	
	serverC.c:

			memset was used to clear the strings as lots of garbage character was being retained and caused errors in code.

			Here I used a Structure called 'credentials' having two attributes:
				1. username[50] - maximum 50 chars (STRING)
				2. password[50] - maximum 50 chars (STRING)

			The structre is capable of holding 300 student's Information.

			The server receives the Encrypted username and Encrypted password from the serverM.

			I used three flags:
				1. 'u' 		- username match
				2. 'p' 		- password match
				3. 'status' - both username and password matched.

			The Credential server first matches the username, if matched the flag 'u' is changed to 1, Only if 'u' is 1, the Password is searched for corresponding user. If the Password also Match, the flag 'p' is updated as 1. If both 'u' and 'p' are 1, the 'status' flag is updated as 1 to confirm "PASS" authentication result.

			I then send the 'result' which is a STRING used to store the authentication result back to the Main server to continue with the process.

			NOTE: The serverC (Credential server) AUTOMATICALLY SHUTS OFF after a successful authntication as it is no longer required to run in the background. It also shuts off AUTOMATICALLY if the number of attempts exceed 3.


	serverEE/serverCS:

			Both serverCS and serverEE contain the EXACT same code with the naming changes to depict 'CS' and 'EE' departments. Their functions REMAIN THE SAME.

			memset was used to clear the strings as lots of garbage character was being retained and caused errors in code.

			MAX is a standard buffer/string length size of 256.

			I used a structure called 'CS' (or EE) to store the values of the "cs.txt" (or ee,txt). the structre is capable of holding upto 300 courses and their information.
			The structure's attributes are:
				1. CourseCode[MAX]	(STRING)
				2. Prof[MAX]		(STRING)
				3. Days[MAX]		(STRING)
				4. CourseName[MAX]	(STRING)
				5. Credits 			(INTEGER)

			The same structure is used for both 'CS' and 'EE' files.

			strtok() is a function used to tokenize a given length of string till a specified character. In our case a comma ','.

			The Coursecode and Category are received firstly, as STRINGS, I use an INTEGER called 'cat' for encoding various categories:
				'Credit'	- 1
				'Professor' - 2
				'Days'		- 3
				'Coursename'- 4

			Then the Program searches the requested informatiion by matching the coursecode with the requested coursecode.

			If the coursecode doesnt Match an error is sent to the Main server else the actual Information requested is sent to the main server.

			NOTE: Choice is received in this server. It ONLY closes the last requested server (EE/CS) and the other running server has to be MANUALLY shut off by the TAs using ctrl+z.


	Makefile:

			It is a file containing the compiling codes of all the files, It creates an executable as mentioned by the Problem Statement.
			Command: 'make all'


3.	Idiosyncrasy:
	
			After complete checking of the Project, I'm Happy to inform that the project doesn't fail under any conditions.
			
4.	Reused Code:
			
			1.	No code was reused from any project on the internet/copied.
			2.	Syntaxes for UDP and TCP connections were refered from Beej's Handout.
			3.	Additional useage of Pre-defined functions are mentioned in the Readme file as well as commented in the code.

----------------------------------------------------------------------------
