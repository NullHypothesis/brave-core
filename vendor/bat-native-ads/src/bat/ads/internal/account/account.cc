/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/account/account.h"

#include "bat/ads/internal/account/ad_rewards/ad_rewards.h"
#include "bat/ads/internal/account/statement/statement.h"
#include "bat/ads/internal/account/transactions/transactions.h"
#include "bat/ads/internal/account/wallet/wallet.h"
#include "bat/ads/internal/account/wallet/wallet_info.h"
#include "bat/ads/internal/confirmations/confirmations_state.h"
#include "bat/ads/internal/confirmations/confirmations.h"
#include "bat/ads/internal/logging.h"
#include "bat/ads/internal/privacy/unblinded_tokens/unblinded_tokens.h"
#include "bat/ads/internal/tokens/redeem_unblinded_payment_tokens/redeem_unblinded_payment_tokens.h"
#include "bat/ads/internal/tokens/refill_unblinded_tokens/refill_unblinded_tokens.h"

namespace ads {

Account::Account(
    Confirmations* confirmations)
    : confirmations_(confirmations),
      wallet_(std::make_unique<Wallet>()),
      ad_rewards_(std::make_unique<AdRewards>()),
      redeem_unblinded_payment_tokens_(std::make_unique<
          RedeemUnblindedPaymentTokens>()),
      refill_unblinded_tokens_(std::make_unique<RefillUnblindedTokens>()) {
  DCHECK(confirmations_);

  confirmations_->AddObserver(this);

  // TODO(https://github.com/brave/brave-browser/issues/12563): Decouple Brave
  // Ads rewards state from confirmations
  confirmations_->set_ad_rewards(ad_rewards_.get());

  ad_rewards_->set_delegate(this);
  redeem_unblinded_payment_tokens_->set_delegate(this);
}

Account::~Account() {
  confirmations_->RemoveObserver(this);
  ad_rewards_->set_delegate(nullptr);
  redeem_unblinded_payment_tokens_->set_delegate(nullptr);
}

void Account::AddObserver(
    AccountObserver* observer) {
  observers_.AddObserver(observer);
}

void Account::RemoveObserver(
    AccountObserver* observer) {
  observers_.RemoveObserver(observer);
}

bool Account::SetWallet(
    const std::string& id,
    const std::string& seed) {
  const WalletInfo last_wallet = wallet_->Get();

  if (!wallet_->Set(id, seed)) {
    BLOG(0, "Invalid wallet");
    NotifyWalletInvalid();
    return false;
  }

  const WalletInfo wallet = wallet_->Get();

  if (last_wallet.IsValid() && last_wallet != wallet) {
    NotifyWalletRestored(wallet);
  }

  NotifyWalletChanged(wallet);

  return true;
}

WalletInfo Account::GetWallet() const {
  return wallet_->Get();
}

StatementInfo Account::GetStatement(
    const int64_t from_timestamp,
    const int64_t to_timestamp) const {
  DCHECK(to_timestamp >= from_timestamp);

  StatementInfo statement;

  statement.estimated_pending_rewards =
      ad_rewards_->GetEstimatedPendingRewards();

  statement.next_payment_date_in_seconds =
      ad_rewards_->GetNextPaymentDateInSeconds();

  statement.ad_notifications_received_this_month =
      ad_rewards_->GetAdNotificationsReceivedThisMonth();

  statement.transactions =
      transactions::GetCleared(from_timestamp, to_timestamp);

  return statement;
}

void Account::Reconcile() {
  const WalletInfo wallet = GetWallet();
  ad_rewards_->MaybeReconcile(wallet);
}

void Account::ProcessUnclearedTransactions() {
  const WalletInfo wallet = GetWallet();
  redeem_unblinded_payment_tokens_->MaybeRedeemAfterDelay(wallet);
}

void Account::TopUpUnblindedTokens() {
  const WalletInfo wallet = GetWallet();
  refill_unblinded_tokens_->MaybeRefill(wallet);
}

///////////////////////////////////////////////////////////////////////////////

void Account::NotifyWalletChanged(
    const WalletInfo& wallet) {
  for (AccountObserver& observer : observers_) {
    observer.OnWalletChanged(wallet);
  }
}

void Account::NotifyWalletRestored(
    const WalletInfo& wallet) {
  for (AccountObserver& observer : observers_) {
    observer.OnWalletRestored(wallet);
  }
}

void Account::NotifyWalletInvalid() {
  for (AccountObserver& observer : observers_) {
    observer.OnWalletInvalid();
  }
}

void Account::NotifyAdRewardsChanged() {
  for (AccountObserver& observer : observers_) {
    observer.OnAdRewardsChanged();
  }
}

void Account::NotifyTransactionsChanged() {
  for (AccountObserver& observer : observers_) {
    observer.OnTransactionsChanged();
  }
}

void Account::NotifyUnclearedTransactionsProcessed() {
  for (AccountObserver& observer : observers_) {
    observer.OnUnclearedTransactionsProcessed();
  }
}

void Account::OnConfirmAd(
    const double estimated_redemption_value,
    const ConfirmationInfo& confirmation) {
  transactions::Add(estimated_redemption_value, confirmation);
  NotifyTransactionsChanged();

  TopUpUnblindedTokens();
}

void Account::OnConfirmAdFailed(
    const ConfirmationInfo& confirmation) {
  TopUpUnblindedTokens();

  confirmations_->RetryAfterDelay();
}

void Account::OnDidReconcileAdRewards() {
  NotifyAdRewardsChanged();
}

void Account::OnDidRedeemUnblindedPaymentTokens() {
  BLOG(1, "Successfully redeemed unblinded payment tokens");

  if (ConfirmationsState::Get()->get_unblinded_payment_tokens()->IsEmpty()) {
    return;
  }

  const TransactionList transactions = transactions::GetUncleared();
  ad_rewards_->SetUnreconciledTransactions(transactions);

  ConfirmationsState::Get()->get_unblinded_payment_tokens()->RemoveAllTokens();
  ConfirmationsState::Get()->Save();

  Reconcile();
}

void Account::OnFailedToRedeemUnblindedPaymentTokens() {
  BLOG(1, "Failed to redeem unblinded payment tokens");
}

void Account::OnDidRetryRedeemingUnblindedPaymentTokens() {
  BLOG(1, "Retry redeeming unblinded payment tokens");
}

}  // namespace ads
