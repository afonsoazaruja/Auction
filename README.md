# Computer Networks

## Project Specification

### User Application (User)

The program implementing the users of the auction platform (User) is invoked using:
`
./user [-n ASIP] [-p ASport]
`

- ASIP: IP address of the machine where the auction server (AS) runs (optional).
- ASport: Well-known port (TCP and UDP) where the AS accepts requests (optional, assumes the value 58000+GN if omitted).

#### User Commands
`
login UID password:
`
- Sends a message to the AS, using UDP, to confirm the ID, UID, and password of the user. Displays the result: successful login, incorrect login attempt, or new user registered.

`
logout:
`
- Sends a message to the AS, using UDP, asking to logout the currently logged-in user. Displays the result: successful logout, unknown user, or user not logged in.

`
unregister:
`
- Sends a message to the AS, using UDP, asking to unregister the currently logged-in user. A logout operation is also performed. Displays the result: successful unregister, unknown user, or incorrect unregister attempt.

`
exit:
`
- Requests to exit the User application. If a user is still logged in, informs the user to first execute the logout command. Otherwise, terminates the application. Local command, not involving communication with the AS.

`
open name asset_fname start_value timeactive:
`
- Establishes a TCP session with the AS and sends a message asking to open a new auction. Displays the result and the assigned auction identifier (AID).

`
close AID:
`
- Sends a message to the AS, using TCP, asking to close an ongoing auction. Displays the result.

`
myauctions or ma:
`
- Sends a message to the AS, using UDP, asking for a list of auctions started by the logged-in user. Displays the result.

`
mybids or mb:
`
- Sends a message to the AS, using UDP, asking for a list of auctions for which the logged-in user has placed a bid. Displays the result.

`
list or l:
`
- Sends a message to the AS, using UDP, asking for a list of auctions. Displays the result.

`
show_asset AID or sa AID:
`
- Establishes a TCP session with the AS and sends a message asking to receive the image file of the asset in sale for auction number AID. Displays the result.

`
bid AID value or b AID value:
`
- Sends a message to the AS, using TCP, asking to place a bid for auction AID of value value. Displays the result.

`
show_record AID or sr AID:
`
- Sends a message to the AS, using UDP, asking to see the record of auction AID. Displays the result.

#### Auction Server (AS)

The program implementing the Auction Server (AS) is invoked with the command:
`
./AS [-p ASport] [-v]
`

    ASport: Well-known port where the AS server accepts requests, both in UDP and TCP (optional, assumes the value 58000+GN if omitted).
    If the -v option is set when invoking the program, it operates in verbose mode.

#### User–AS Protocol (in UDP)

Interaction between the User application (User) and the auction server (AS) is supported by the UDP protocol. The related request and reply protocol messages include:

    LIN UID password: Login command.
    RLI status: Reply to login request.
    LOU UID password: Logout command.
    RLO status: Reply to logout request.
    UNR UID password: Unregister command.
    RUR status: Reply to unregister request.
    LMA UID: My auctions command.
    RMA status[ AID state]*: Reply to my auctions request.
    LMB UID: My bids command.
    RMB status[ AID state]*: Reply to my bids request.
    LST: List command.
    RLS status[ AID state]*: Reply to list request.
    SRC AID: Show record command.
    RRC status [host_UID auction_name asset_fname start_value start_date-time timeactive] [ B bidder_UID bid_value bid_date-time bid_sec_time]* [ E end_date-time end_sec_time]: Reply to show record request.


#### User–AS Messaging Protocol (in TCP)

Interaction between the User application (User) and the auction server (AS) for messaging related to the transfer of files is supported by the TCP protocol. The related request and reply protocol messages include:

    OPA UID password name start_value timeactive Fname Fsize Fdata: Open command.
    ROA status [AID]: Reply to open request.
    CLS UID password AID: Close command.
    RCL status: Reply to close request.
    SAS AID: Show asset command.
    RSA status [Fname Fsize Fdata]: Reply to show asset request.
    BID UID password AID value: Bid command.
    RBD status: Reply to bid request.

Note: Filenames (Fname) are limited to 24 alphanumerical characters (plus ‘-‘, ‘_’, and ‘.’), including the separating dot and the 3-letter extension: “nnn...nnnn.xxx”. The file size (Fsize) is limited to 10 MB (10.106 B).