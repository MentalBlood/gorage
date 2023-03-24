#pragma once

#ifndef __GORAGE__INTEGRITY_STORAGE__
#define __GORAGE__INTEGRITY_STORAGE__

#include <vector>
#include <memory>

#include "Storage.hpp"



namespace gorage {

	template<class T, class I>
	class IntegrityStorage : public Storage<T> {

	public:

		IntegrityStorage(std::shared_ptr<Storage<T>> base, std::shared_ptr<Storage<I>> integrity):
			_base(base),
			_integrity(integrity) {}

		void save(const std::string& usi, const T& content) {
			_integrity->save(digest_usi(usi), digest(content));
			_base->save(usi, content);
		}

		T load(const std::string& usi) const {
			const T result = _base->load(usi);
			const I result_digest = digest(result);
			const I stated_digest = _integrity->load(digest_usi(usi));
			if (result_digest != stated_digest) {
				throw OperationalError("Can not load object with usi `" + usi + "`: integrity check failed");
			}
			return result;
		}

		void remove(const std::string& usi) {
			_base->remove(usi);
			_integrity->remove(digest_usi(usi));
		}

	protected:

		virtual I digest(const T& content) const = 0;

		void loadUsis() {
			_usis = std::unordered_set<std::string>(_base->begin(), _base->end());
		}

		virtual std::string digest_usi(const std::string usi) const {
			return usi + "_digest";
		}

	private:

		std::shared_ptr<Storage<T>> _base;
		std::shared_ptr<Storage<I>> _integrity;

	};

} // gorage

#endif