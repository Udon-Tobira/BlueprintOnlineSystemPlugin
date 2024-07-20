# BlueprintOnlineSystemPlugin

Plugin to make online system functionality easily usable from blueprints.
The following blueprint callable functions are available:
- Login
  
  ![image](https://github.com/user-attachments/assets/339309d9-47ad-469f-a59a-0210b4113be6)
  
  - inputs
    - Player Controller: A player controller to be logged in
    - Auth Type: Authentication Type. Basically "accountportal"?

- Create Session
  
  ![image](https://github.com/user-attachments/assets/b9620f2d-e275-4cce-b6e7-8ea78c785506)
  
  - inputs
    - Player Controller: A player controller for session creation
    - Max Connections: Maximum number of people that can be connected
  - outputs
    - Session Id: Session id required to join a session in "Find Session"

- Destroy Session
  
  ![image](https://github.com/user-attachments/assets/7aa8b7df-e067-4072-9ca7-74618e169c59)
  
  If the session creator invokes this, the session is terminated and all participants are 
  withdrawn from the session. If the participant of the session invokes this, he/she simply leaves 
  the session himself/herself only.

- Find Session
  
  ![image](https://github.com/user-attachments/assets/b1155039-2cad-4647-aad9-e87aefcfaba4)

  - inputs
    - Player Controller: Player controller for session creation
    - Session Id: Session id to be searched for. You need to get the session id from the person who executed Create Session by some means.
  - outputs
    - Blueprint Session Search Result: A session search result information. Used in the Join Session function.

- Join Session
  
  ![image](https://github.com/user-attachments/assets/ff340685-a118-47ed-bb05-2d876960cc9f)
  
  Join in a session. However, joining a session does not mean moving to a level. To move to the level where the server is located, run the Open Level (by Name) blueprint node with Connect Info at the "Level Name" property and a check the "Absolute" property.
  - inputs
    - Player Controller: A Player controller to join in the session.
    - Blueprint Session Search Result: A session search result; use the "Find Session" function search result.
  - outputs
    - Connect Info: Level and address information for the connection.
