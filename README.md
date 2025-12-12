Some Beacon Object Files I put together for use & experimentation with [Sliver](https://sliver.sh/docs?name=BOF+and+COFF+Support).

## Commands 
|Commands|Usage|Notes|
|--------|-----|-----|
|check_autologon | check_autologon| Simple registry check for AutoAdminLogon and Default* in Winlogon|
|edit_lnk | edit_lnk [shortcut path] [target path] [opt:args] [opt:working dir]| Edit the properties of a LNK/shortcut using the IShellLinkW and IPersistFile COM objects|
|enum_adapters | enum_adapters| Enumerate adapters and adapter bindings using the INetCfg* and IEnumNet* COM objects|
|enum_interfaces | enum_interfaces| Enumerate interface information using [IP helper](https://learn.microsoft.com/en-us/windows/win32/api/_iphlp/)|
|get_bootkey | get_bootkey| Dump and compute the boot key used to decrypt SAM and LSA secrets. Converted from this [gist](https://gist.github.com/Dfte/3462d0a08af57392e1629b8c83021155). [Ref:](https://www.orangecyberdefense.com/global/blog/cybersecurity/bypassing-edr-to-dump-lsa-secrets)|
|time_roast | time_roast [DC IP]| Abuses the legacy MS-SNTP authentication extension. Converted from and based on the [scripts](https://github.com/SecuraBV/Timeroast) from Tom Tervoort and accompanying [whitepaper](https://cybersecurity.bureauveritas.com/uploads/whitepapers/Secura-WP-Timeroasting-v3.pdf)|
|toggle_binding | toggle_binding [adapter ID] [binding ID] [action]| Disable a binding on a specified adapter using the INetCfg* and IEnumNet* COM objects|
|toggle_if_as | toggle_if_as [interface index number] [action]| Set the `AdminStatus` of an interface to up or down|
