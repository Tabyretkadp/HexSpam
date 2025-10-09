<div align="center">

<h1>Hex Spam v1.0</h1><p><em>Telegram Phone Number Validator</em></p>

</div>

***Hex Spam*** is a powerful console application designed to validate phone numbers against Telegram's database. It helps users identify which phone numbers have registered Telegram accounts by leveraging Telegram's contact addition feature.

# Screenshots

<img src="screen/Screenshot_20250912_115420.png" alt="Screen" width="500"/>
<img src="screen/Screenshot_20250912_115314.png" alt="Screen" width="500"/>

## Features

- **Phone Number Validatio**n: Automatically checks if phone numbers are registered on Telegram
- **Logging System**: Comprehensive logging of valid and invalid numbers
- **Proxy Support**: Optional proxy configuration for enhanced privacy
- **Console Control**: Simple and convenient menu for managing activities.

---

>[!WARNING]
> ⚠️
> Warning: Use at your own risk!
> - Always use throwaway or temporary Telegram accounts
> - Multiple rapid contact additions may trigger Telegram's anti-spam systems
> - Accounts may face temporary or permanent restriction
> - Recommended to use with proxies for better safety

## Dependencies Installation

> If something doesn't work, then solve the problem yourself) I'll deal with this problem later

### 1. Install Qt6:
```bash
sudo pacman -S qt6-base qt6-tools
```
### 2. Install Project:
```bash
git clone https://github.com/Tabyretkadp/.chronix.git
cd .chronix
```
### 3. Install TDlib
```bash
Download TDLib -> https://tdlib.github.io/td/build.html
```
### 4. Install Other
```bash
mkdir build
cd build
cmake ..
make
```
