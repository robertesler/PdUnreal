# PdUnreal
A demonstration of how to use Pd++ with Unreal Engine 5 and Metasounds
Currently, in development.  If you really want to try it right now create a new Plugin in Unreal.  Make sure to add the folowing to your .uplugin file:
```
    "Plugins": [
 
        {
 
            "Name": "Metasound",
 
            "Enabled": true
 
        }
 
    ]
```
Then you can copy the source to your project.  Hopefully it will work.  
Coming soon: tutorials, more examples and a much better readme file. : )

## Tutorials
Here is a comprehensive tutorial on how I got this to work on MacOS: https://www.youtube.com/watch?v=KNTKdsBIrqQ&t=2s.  It was tricky to get this to work on Mac.  Less so on Windows, but it takes some configuring of Visual Studio to work with Unreal.  I kept some notes on numerous problems I had working with Unreal Plugins and Metasounds here: https://github.com/robertesler/PdUnreal/blob/main/PdPlugin/UnrealNotes.txt 
