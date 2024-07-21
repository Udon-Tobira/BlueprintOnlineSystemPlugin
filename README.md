# BlueprintOnlineSystemPlugin

Plugin to make online system functionality easily usable from blueprints.

## What you'll be able to do.
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

## How to install
Clone this repository into the Plugins folder (or create your own if you don't have one) in the folder of the Unreal Engine project where you want to install this plugin by doing one of the following:
<details>
<summary>Intallation method 1 (using Github Desktop) (easy)</summary>

1. Launch "Github Desktop" application (if not available, install it first).
2. From the menu, select File > Clone repository...
3. Go to the URL tab.
4. Enter the URL of this Github repository in "URL or username/repository", select the Plugins folder of the project where you want to install this plugin in "Local path", and press Clone.
</details>

<details>
<summary>Intallation method 2 (using git command) (advanced)</summary>

1. If git is not installed, please install it.
2. In the Plugins folder of the project where you want to install this plug-in, start a command prompt.
3. Execute  
   ```
   git clone URL`
   ```
   Put the URL of this repository in the URL field.
</details>

After installing the plugin using one of the above procedures, open the project (Press Yes when you see "The following modules are missing or built with a different engine version: RuntimeAssetImport") and the plugin is enabled.

## How to use
Run [Blueprint Online System Plugin Sample]([https://github.com/Udon-Tobira/RuntimeAssImpSample](https://github.com/Udon-Tobira/BPOSPluginSample)).
