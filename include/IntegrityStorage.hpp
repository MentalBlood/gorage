#pragma once

#ifndef __GORAGE__BINARY_FILE_STORAGE__
#define __GORAGE__BINARY_FILE_STORAGE__

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

	protected:

		virtual I digest(const T& content) = 0;

		void save(const std::string& usi, const T& content) {
			_integrity->save(usi, digest(content));
			_base->save(usi, content);
		}

		T load(const std::string& usi) {
			const T result = _base->load(usi);
			const I result_digest = digest(result);
			const I stated_digest = _integrity->load(usi);
			if (result_digest != stated_digest) {
				throw std::runtime_error("Can not load object with usi `" + usi + "`: integrity check failed");
			}
		}

		T remove(const std::string& usi) {
			_base->remove(usi);
			_integrity->remove(usi);
		}

	private:

		std::shared_ptr<Storage<T>> _base;
		std::shared_ptr<Storage<I>> _integrity;

		void loadUsis() {}

	};

} // gorage

#endif