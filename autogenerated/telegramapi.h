#ifndef TELEGRAMAPI_H
#define TELEGRAMAPI_H

// Generated by APIGenerator 1.0
// DO NOT EDIT!!!

#include "../mtproto/dc/dcsession.h"
#include "../types/basic.h"
#include <QObject>
#include "../autogenerated/mtproto/mtproto.h"
#include "../mtproto/mtprotorequest.h"
#include "../autogenerated/types/types.h"

class TelegramAPI: public QObject
{
	Q_OBJECT

	public:
		explicit TelegramAPI(QObject* parent = 0);
		static MTProtoRequest* authCheckPhone(DCSession* session, TLString phone_number);
		static MTProtoRequest* authSendCode(DCSession* session, TLString phone_number, TLBool is_current_number, TLInt api_id, TLString api_hash);
		static MTProtoRequest* authSignUp(DCSession* session, TLString phone_number, TLString phone_code_hash, TLString phone_code, TLString first_name, TLString last_name);
		static MTProtoRequest* authSignIn(DCSession* session, TLString phone_number, TLString phone_code_hash, TLString phone_code);
		static MTProtoRequest* authLogOut(DCSession* session);
		static MTProtoRequest* authResetAuthorizations(DCSession* session);
		static MTProtoRequest* authSendInvites(DCSession* session, TLVector<TLString> phone_numbers, TLString message);
		static MTProtoRequest* authExportAuthorization(DCSession* session, TLInt dc_id);
		static MTProtoRequest* authImportAuthorization(DCSession* session, TLInt id, TLBytes bytes);
		static MTProtoRequest* authBindTempAuthKey(DCSession* session, TLLong perm_auth_key_id, TLLong nonce, TLInt expires_at, TLBytes encrypted_message);
		static MTProtoRequest* authImportBotAuthorization(DCSession* session, TLInt flags, TLInt api_id, TLString api_hash, TLString bot_auth_token);
		static MTProtoRequest* authCheckPassword(DCSession* session, TLBytes password_hash);
		static MTProtoRequest* authRequestPasswordRecovery(DCSession* session);
		static MTProtoRequest* authRecoverPassword(DCSession* session, TLString code);
		static MTProtoRequest* authResendCode(DCSession* session, TLString phone_number, TLString phone_code_hash);
		static MTProtoRequest* authCancelCode(DCSession* session, TLString phone_number, TLString phone_code_hash);
		static MTProtoRequest* authDropTempAuthKeys(DCSession* session, TLVector<TLLong> except_auth_keys);
		static MTProtoRequest* accountRegisterDevice(DCSession* session, TLInt token_type, TLString token);
		static MTProtoRequest* accountUnregisterDevice(DCSession* session, TLInt token_type, TLString token);
		static MTProtoRequest* accountUpdateNotifySettings(DCSession* session, InputNotifyPeer* peer, InputPeerNotifySettings* settings);
		static MTProtoRequest* accountGetNotifySettings(DCSession* session, InputNotifyPeer* peer);
		static MTProtoRequest* accountResetNotifySettings(DCSession* session);
		static MTProtoRequest* accountUpdateProfile(DCSession* session, TLString first_name, TLString last_name, TLString about);
		static MTProtoRequest* accountUpdateStatus(DCSession* session, TLBool is_offline);
		static MTProtoRequest* accountGetWallPapers(DCSession* session);
		static MTProtoRequest* accountReportPeer(DCSession* session, InputPeer* peer, ReportReason* reason);
		static MTProtoRequest* accountCheckUsername(DCSession* session, TLString username);
		static MTProtoRequest* accountUpdateUsername(DCSession* session, TLString username);
		static MTProtoRequest* accountGetPrivacy(DCSession* session, InputPrivacyKey* key);
		static MTProtoRequest* accountSetPrivacy(DCSession* session, InputPrivacyKey* key, TLVector<InputPrivacyRule*> rules);
		static MTProtoRequest* accountDeleteAccount(DCSession* session, TLString reason);
		static MTProtoRequest* accountGetAccountTTL(DCSession* session);
		static MTProtoRequest* accountSetAccountTTL(DCSession* session, AccountDaysTTL* ttl);
		static MTProtoRequest* accountSendChangePhoneCode(DCSession* session, TLString phone_number, TLBool is_current_number);
		static MTProtoRequest* accountChangePhone(DCSession* session, TLString phone_number, TLString phone_code_hash, TLString phone_code);
		static MTProtoRequest* accountUpdateDeviceLocked(DCSession* session, TLInt period);
		static MTProtoRequest* accountGetAuthorizations(DCSession* session);
		static MTProtoRequest* accountResetAuthorization(DCSession* session, TLLong hash);
		static MTProtoRequest* accountGetPassword(DCSession* session);
		static MTProtoRequest* accountGetPasswordSettings(DCSession* session, TLBytes current_password_hash);
		static MTProtoRequest* accountUpdatePasswordSettings(DCSession* session, TLBytes current_password_hash, AccountPasswordInputSettings* new_settings);
		static MTProtoRequest* accountSendConfirmPhoneCode(DCSession* session, TLString hash, TLBool is_current_number);
		static MTProtoRequest* accountConfirmPhone(DCSession* session, TLString phone_code_hash, TLString phone_code);
		static MTProtoRequest* usersGetUsers(DCSession* session, TLVector<InputUser*> id);
		static MTProtoRequest* usersGetFullUser(DCSession* session, InputUser* id);
		static MTProtoRequest* contactsGetStatuses(DCSession* session);
		static MTProtoRequest* contactsGetContacts(DCSession* session, TLString hash);
		static MTProtoRequest* contactsImportContacts(DCSession* session, TLVector<InputContact*> contacts, TLBool is_replace);
		static MTProtoRequest* contactsDeleteContact(DCSession* session, InputUser* id);
		static MTProtoRequest* contactsDeleteContacts(DCSession* session, TLVector<InputUser*> id);
		static MTProtoRequest* contactsBlock(DCSession* session, InputUser* id);
		static MTProtoRequest* contactsUnblock(DCSession* session, InputUser* id);
		static MTProtoRequest* contactsGetBlocked(DCSession* session, TLInt offset, TLInt limit);
		static MTProtoRequest* contactsExportCard(DCSession* session);
		static MTProtoRequest* contactsImportCard(DCSession* session, TLVector<TLInt> export_card);
		static MTProtoRequest* contactsSearch(DCSession* session, TLString q, TLInt limit);
		static MTProtoRequest* contactsResolveUsername(DCSession* session, TLString username);
		static MTProtoRequest* contactsGetTopPeers(DCSession* session, TLInt offset, TLInt limit, TLInt hash);
		static MTProtoRequest* contactsResetTopPeerRating(DCSession* session, TopPeerCategory* category, InputPeer* peer);
		static MTProtoRequest* messagesGetMessages(DCSession* session, TLVector<TLInt> id);
		static MTProtoRequest* messagesGetDialogs(DCSession* session, TLInt offset_date, TLInt offset_id, InputPeer* offset_peer, TLInt limit);
		static MTProtoRequest* messagesGetHistory(DCSession* session, InputPeer* peer, TLInt offset_id, TLInt offset_date, TLInt add_offset, TLInt limit, TLInt max_id, TLInt min_id);
		static MTProtoRequest* messagesSearch(DCSession* session, InputPeer* peer, TLString q, MessagesFilter* filter, TLInt min_date, TLInt max_date, TLInt offset, TLInt max_id, TLInt limit);
		static MTProtoRequest* messagesReadHistory(DCSession* session, InputPeer* peer, TLInt max_id);
		static MTProtoRequest* messagesDeleteHistory(DCSession* session, InputPeer* peer, TLInt max_id);
		static MTProtoRequest* messagesDeleteMessages(DCSession* session, TLVector<TLInt> id);
		static MTProtoRequest* messagesReceivedMessages(DCSession* session, TLInt max_id);
		static MTProtoRequest* messagesSetTyping(DCSession* session, InputPeer* peer, SendMessageAction* action);
		static MTProtoRequest* messagesSendMessage(DCSession* session, InputPeer* peer, TLInt reply_to_msg_id, TLString message, TLLong random_id, ReplyMarkup* reply_markup, TLVector<MessageEntity*> entities);
		static MTProtoRequest* messagesSendMedia(DCSession* session, InputPeer* peer, TLInt reply_to_msg_id, InputMedia* media, TLLong random_id, ReplyMarkup* reply_markup);
		static MTProtoRequest* messagesForwardMessages(DCSession* session, InputPeer* from_peer, TLVector<TLInt> id, TLVector<TLLong> random_id, InputPeer* to_peer);
		static MTProtoRequest* messagesReportSpam(DCSession* session, InputPeer* peer);
		static MTProtoRequest* messagesHideReportSpam(DCSession* session, InputPeer* peer);
		static MTProtoRequest* messagesGetPeerSettings(DCSession* session, InputPeer* peer);
		static MTProtoRequest* messagesGetChats(DCSession* session, TLVector<TLInt> id);
		static MTProtoRequest* messagesGetFullChat(DCSession* session, TLInt chat_id);
		static MTProtoRequest* messagesEditChatTitle(DCSession* session, TLInt chat_id, TLString title);
		static MTProtoRequest* messagesEditChatPhoto(DCSession* session, TLInt chat_id, InputChatPhoto* photo);
		static MTProtoRequest* messagesAddChatUser(DCSession* session, TLInt chat_id, InputUser* user_id, TLInt fwd_limit);
		static MTProtoRequest* messagesDeleteChatUser(DCSession* session, TLInt chat_id, InputUser* user_id);
		static MTProtoRequest* messagesCreateChat(DCSession* session, TLVector<InputUser*> users, TLString title);
		static MTProtoRequest* messagesForwardMessage(DCSession* session, InputPeer* peer, TLInt id, TLLong random_id);
		static MTProtoRequest* messagesGetDhConfig(DCSession* session, TLInt version, TLInt random_length);
		static MTProtoRequest* messagesRequestEncryption(DCSession* session, InputUser* user_id, TLInt random_id, TLBytes g_a);
		static MTProtoRequest* messagesAcceptEncryption(DCSession* session, InputEncryptedChat* peer, TLBytes g_b, TLLong key_fingerprint);
		static MTProtoRequest* messagesDiscardEncryption(DCSession* session, TLInt chat_id);
		static MTProtoRequest* messagesSetEncryptedTyping(DCSession* session, InputEncryptedChat* peer, TLBool is_typing);
		static MTProtoRequest* messagesReadEncryptedHistory(DCSession* session, InputEncryptedChat* peer, TLInt max_date);
		static MTProtoRequest* messagesSendEncrypted(DCSession* session, InputEncryptedChat* peer, TLLong random_id, TLBytes data);
		static MTProtoRequest* messagesSendEncryptedFile(DCSession* session, InputEncryptedChat* peer, TLLong random_id, TLBytes data, InputEncryptedFile* file);
		static MTProtoRequest* messagesSendEncryptedService(DCSession* session, InputEncryptedChat* peer, TLLong random_id, TLBytes data);
		static MTProtoRequest* messagesReceivedQueue(DCSession* session, TLInt max_qts);
		static MTProtoRequest* messagesReadMessageContents(DCSession* session, TLVector<TLInt> id);
		static MTProtoRequest* messagesGetAllStickers(DCSession* session, TLInt hash);
		static MTProtoRequest* messagesGetWebPagePreview(DCSession* session, TLString message);
		static MTProtoRequest* messagesExportChatInvite(DCSession* session, TLInt chat_id);
		static MTProtoRequest* messagesCheckChatInvite(DCSession* session, TLString hash);
		static MTProtoRequest* messagesImportChatInvite(DCSession* session, TLString hash);
		static MTProtoRequest* messagesGetStickerSet(DCSession* session, InputStickerSet* stickerset);
		static MTProtoRequest* messagesInstallStickerSet(DCSession* session, InputStickerSet* stickerset, TLBool is_archived);
		static MTProtoRequest* messagesUninstallStickerSet(DCSession* session, InputStickerSet* stickerset);
		static MTProtoRequest* messagesStartBot(DCSession* session, InputUser* bot, InputPeer* peer, TLLong random_id, TLString start_param);
		static MTProtoRequest* messagesGetMessagesViews(DCSession* session, InputPeer* peer, TLVector<TLInt> id, TLBool is_increment);
		static MTProtoRequest* messagesToggleChatAdmins(DCSession* session, TLInt chat_id, TLBool is_enabled);
		static MTProtoRequest* messagesEditChatAdmin(DCSession* session, TLInt chat_id, InputUser* user_id, TLBool is_admin);
		static MTProtoRequest* messagesMigrateChat(DCSession* session, TLInt chat_id);
		static MTProtoRequest* messagesSearchGlobal(DCSession* session, TLString q, TLInt offset_date, InputPeer* offset_peer, TLInt offset_id, TLInt limit);
		static MTProtoRequest* messagesReorderStickerSets(DCSession* session, TLVector<TLLong> order);
		static MTProtoRequest* messagesGetDocumentByHash(DCSession* session, TLBytes sha256, TLInt size, TLString mime_type);
		static MTProtoRequest* messagesSearchGifs(DCSession* session, TLString q, TLInt offset);
		static MTProtoRequest* messagesGetSavedGifs(DCSession* session, TLInt hash);
		static MTProtoRequest* messagesSaveGif(DCSession* session, InputDocument* id, TLBool is_unsave);
		static MTProtoRequest* messagesGetInlineBotResults(DCSession* session, InputUser* bot, InputPeer* peer, InputGeoPoint* geo_point, TLString query, TLString offset);
		static MTProtoRequest* messagesSetInlineBotResults(DCSession* session, TLLong query_id, TLVector<InputBotInlineResult*> results, TLInt cache_time, TLString next_offset, InlineBotSwitchPM* switch_pm);
		static MTProtoRequest* messagesSendInlineBotResult(DCSession* session, InputPeer* peer, TLInt reply_to_msg_id, TLLong random_id, TLLong query_id, TLString id);
		static MTProtoRequest* messagesGetMessageEditData(DCSession* session, InputPeer* peer, TLInt id);
		static MTProtoRequest* messagesEditMessage(DCSession* session, InputPeer* peer, TLInt id, TLString message, ReplyMarkup* reply_markup, TLVector<MessageEntity*> entities);
		static MTProtoRequest* messagesEditInlineBotMessage(DCSession* session, InputBotInlineMessageID* id, TLString message, ReplyMarkup* reply_markup, TLVector<MessageEntity*> entities);
		static MTProtoRequest* messagesGetBotCallbackAnswer(DCSession* session, InputPeer* peer, TLInt msg_id, TLBytes data);
		static MTProtoRequest* messagesSetBotCallbackAnswer(DCSession* session, TLLong query_id, TLString message, TLString url);
		static MTProtoRequest* messagesGetPeerDialogs(DCSession* session, TLVector<InputPeer*> peers);
		static MTProtoRequest* messagesSaveDraft(DCSession* session, TLInt reply_to_msg_id, InputPeer* peer, TLString message, TLVector<MessageEntity*> entities);
		static MTProtoRequest* messagesGetAllDrafts(DCSession* session);
		static MTProtoRequest* messagesGetFeaturedStickers(DCSession* session, TLInt hash);
		static MTProtoRequest* messagesReadFeaturedStickers(DCSession* session, TLVector<TLLong> id);
		static MTProtoRequest* messagesGetRecentStickers(DCSession* session, TLInt hash);
		static MTProtoRequest* messagesSaveRecentSticker(DCSession* session, InputDocument* id, TLBool is_unsave);
		static MTProtoRequest* messagesClearRecentStickers(DCSession* session);
		static MTProtoRequest* messagesGetArchivedStickers(DCSession* session, TLLong offset_id, TLInt limit);
		static MTProtoRequest* messagesGetMaskStickers(DCSession* session, TLInt hash);
		static MTProtoRequest* messagesGetAttachedStickers(DCSession* session, InputStickeredMedia* media);
		static MTProtoRequest* messagesSetGameScore(DCSession* session, InputPeer* peer, TLInt id, InputUser* user_id, TLInt score);
		static MTProtoRequest* messagesSetInlineGameScore(DCSession* session, InputBotInlineMessageID* id, InputUser* user_id, TLInt score);
		static MTProtoRequest* messagesGetGameHighScores(DCSession* session, InputPeer* peer, TLInt id, InputUser* user_id);
		static MTProtoRequest* messagesGetInlineGameHighScores(DCSession* session, InputBotInlineMessageID* id, InputUser* user_id);
		static MTProtoRequest* updatesGetState(DCSession* session);
		static MTProtoRequest* updatesGetDifference(DCSession* session, TLInt pts, TLInt date, TLInt qts);
		static MTProtoRequest* updatesGetChannelDifference(DCSession* session, InputChannel* channel, ChannelMessagesFilter* filter, TLInt pts, TLInt limit);
		static MTProtoRequest* photosUpdateProfilePhoto(DCSession* session, InputPhoto* id);
		static MTProtoRequest* photosUploadProfilePhoto(DCSession* session, InputFile* file);
		static MTProtoRequest* photosDeletePhotos(DCSession* session, TLVector<InputPhoto*> id);
		static MTProtoRequest* photosGetUserPhotos(DCSession* session, InputUser* user_id, TLInt offset, TLLong max_id, TLInt limit);
		static MTProtoRequest* uploadSaveFilePart(DCSession* session, TLLong file_id, TLInt file_part, TLBytes bytes);
		static MTProtoRequest* uploadGetFile(DCSession* session, InputFileLocation* location, TLInt offset, TLInt limit);
		static MTProtoRequest* uploadSaveBigFilePart(DCSession* session, TLLong file_id, TLInt file_part, TLInt file_total_parts, TLBytes bytes);
		static MTProtoRequest* helpGetConfig(DCSession* session);
		static MTProtoRequest* helpGetNearestDc(DCSession* session);
		static MTProtoRequest* helpGetAppUpdate(DCSession* session);
		static MTProtoRequest* helpSaveAppLog(DCSession* session, TLVector<InputAppEvent*> events);
		static MTProtoRequest* helpGetInviteText(DCSession* session);
		static MTProtoRequest* helpGetSupport(DCSession* session);
		static MTProtoRequest* helpGetAppChangelog(DCSession* session);
		static MTProtoRequest* helpGetTermsOfService(DCSession* session);
		static MTProtoRequest* channelsReadHistory(DCSession* session, InputChannel* channel, TLInt max_id);
		static MTProtoRequest* channelsDeleteMessages(DCSession* session, InputChannel* channel, TLVector<TLInt> id);
		static MTProtoRequest* channelsDeleteUserHistory(DCSession* session, InputChannel* channel, InputUser* user_id);
		static MTProtoRequest* channelsReportSpam(DCSession* session, InputChannel* channel, InputUser* user_id, TLVector<TLInt> id);
		static MTProtoRequest* channelsGetMessages(DCSession* session, InputChannel* channel, TLVector<TLInt> id);
		static MTProtoRequest* channelsGetParticipants(DCSession* session, InputChannel* channel, ChannelParticipantsFilter* filter, TLInt offset, TLInt limit);
		static MTProtoRequest* channelsGetParticipant(DCSession* session, InputChannel* channel, InputUser* user_id);
		static MTProtoRequest* channelsGetChannels(DCSession* session, TLVector<InputChannel*> id);
		static MTProtoRequest* channelsGetFullChannel(DCSession* session, InputChannel* channel);
		static MTProtoRequest* channelsCreateChannel(DCSession* session, TLString title, TLString about);
		static MTProtoRequest* channelsEditAbout(DCSession* session, InputChannel* channel, TLString about);
		static MTProtoRequest* channelsEditAdmin(DCSession* session, InputChannel* channel, InputUser* user_id, ChannelParticipantRole* role);
		static MTProtoRequest* channelsEditTitle(DCSession* session, InputChannel* channel, TLString title);
		static MTProtoRequest* channelsEditPhoto(DCSession* session, InputChannel* channel, InputChatPhoto* photo);
		static MTProtoRequest* channelsCheckUsername(DCSession* session, InputChannel* channel, TLString username);
		static MTProtoRequest* channelsUpdateUsername(DCSession* session, InputChannel* channel, TLString username);
		static MTProtoRequest* channelsJoinChannel(DCSession* session, InputChannel* channel);
		static MTProtoRequest* channelsLeaveChannel(DCSession* session, InputChannel* channel);
		static MTProtoRequest* channelsInviteToChannel(DCSession* session, InputChannel* channel, TLVector<InputUser*> users);
		static MTProtoRequest* channelsKickFromChannel(DCSession* session, InputChannel* channel, InputUser* user_id, TLBool is_kicked);
		static MTProtoRequest* channelsExportInvite(DCSession* session, InputChannel* channel);
		static MTProtoRequest* channelsDeleteChannel(DCSession* session, InputChannel* channel);
		static MTProtoRequest* channelsToggleInvites(DCSession* session, InputChannel* channel, TLBool is_enabled);
		static MTProtoRequest* channelsExportMessageLink(DCSession* session, InputChannel* channel, TLInt id);
		static MTProtoRequest* channelsToggleSignatures(DCSession* session, InputChannel* channel, TLBool is_enabled);
		static MTProtoRequest* channelsUpdatePinnedMessage(DCSession* session, InputChannel* channel, TLInt id);
		static MTProtoRequest* channelsGetAdminedPublicChannels(DCSession* session);

};

#endif // TELEGRAMAPI_H
