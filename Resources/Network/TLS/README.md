# HTTPS credentials

Configure Network with a PEM certificate/chain and matching unencrypted private key. Files here are ignored by Git. For localhost development, run `python Resources/Modules/Appeal_Module_Network/Appeal_SDL2_Network/Packaging/Create_Development_Certificate.py`; its self-signed certificate lasts seven days and is not publicly trusted. Then run `Appeal.exe Examples/Network/HTTPS_Server`.

For public use supply your own valid CA-issued certificate and matching private key, protect the key with OS permissions, and configure the intended host/port. Stop/restart the server when rotating credentials. Do not put keys in the served HTML folder.
