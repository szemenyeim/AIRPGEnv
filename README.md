# AiRPGEnv

 Researchers have achieved significant success in controlling various board games, arena games, and strategy games with Deep Neural Networks in recent years.
  Nevertheless, there have been relatively few attempts to control Role Playing Games, which are perhaps closest to real-life environments conceptually.
  The main goal of this project is to create a pixel-RPG environment for developing and testing Deep Reinforcement Learning algorithms. 
  
  The whole environment is written in python and it is compatible with OpenAI Gym.
  The AIRPGEnv is multiplayer only.
  
## Requirements
  OpenCv
  Gym
  Numpy
  
 ## Usage
  
  ```python
  env = Environment(mode,"playerName",ip,port, visu)
  ```
  ### Parameter:
  - mode      : "human" (playing with keyboard) / "gym" (for algorithms)
  - IP, port  : IP adress and port number for multiplayer
  - visu      : True if you want to show the playground, False othervise
